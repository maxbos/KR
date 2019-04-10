import cloneDeep from 'lodash/cloneDeep';

const DERIVATIVE_SPACE = ['-', '0', '+'];
const DERIVATIVE_SPACE_INT = [-1, 0, 1];

class Quantity {
  /**
   * 
   * @param {Array} space 
   * @param {String} magnitude 
   * @param {String} derivative 
   */
  constructor({ space, magnitude, derivative, dependencies = [] }) {
    this.space = space;
    this.magnitude = magnitude;
    this.derivative = derivative;
    this.dependencies = dependencies;
    this.forcedNextState = null;
    this.propagateCount = 0;
  }

  /**
   * 
   * @param {String} dependencyType 
   * @param {Array} dependencyInfo 
   */
  setDependency(dependencyType, dependencyInfo) {
    this.dependencies.push([dependencyType, dependencyInfo]);
  }

  enableDerivative(type, value) {
    this.forcedNextState = {
      magnitude: this.magnitude,
      derivative: 1,
      space: this.space,
      dependencies: this.dependencies,
    };
  }

  processDerivative(name) {
    if (this.forcedNextState) {
      return [this.forcedNextState, `set derivative of ${name} to 1`];
    }
    let log = '';
    let magnitude = this.magnitude;
    if (this.derivative !== 0) {
      // Change the magnitude based on the derivative integer value, e.g. magnitude
      // of 'max' becomes '+' if derivative is '-' because derivative integer is -1.
      const magnitudeIdx = this.space.numeric.findIndex((e) => e === this.magnitude);
      const newMagnitudeIdx = Math.max(0, Math.min(this.space.numeric.length-1, magnitudeIdx + this.derivative));
      magnitude = this.space.numeric[newMagnitudeIdx];
      log = `∂${name} was ${this.getDerivativeLabel(this.derivative)} so changed magnitude to ${this.getSpaceLabel(magnitude)}`;
    }
    return [{
      magnitude,
      // If magnitude is maximum, the derivative is capped.
      // derivative: this.getValidDerivative(this.derivative, magnitude),
      derivative: this.derivative,
      space: this.space,
      dependencies: this.dependencies,
    }, log];
  }

  /**
   * Perform value constraints if a constraint dependency exists in this
   * quantity. Otherwise, return the current set of base states.
   */
  valueConstraint(baseState) {
    for (let i = 0; i < this.dependencies.length; i++) {
      const dep = this.dependencies[i];
      if (dep[0] === 'valueConstraint') {
        const [ quantityName, quantityValue, dependentQuantityName, dependentValue ] = dep[1];
        // If this quantity contains a value constraint, perform that constraint.
        // So get the value from quantity, if that value is equal to the conditional value,
        // set the dependent quantity to the dependent value.
        if (baseState.quantities[quantityName].magnitude === this.getSpaceNumeric(quantityValue)) {
          return {
            quantities: {
              ...baseState.quantities,
              [dependentQuantityName]: {
                ...baseState.quantities[dependentQuantityName],
                magnitude: this.getSpaceNumeric(dependentValue),
              },
            },
            log: [
              ...baseState.log,
              `VC: set ${dependentQuantityName} to ${dependentValue} because ${quantityName} is ${quantityValue}`,
            ],
          };
        }
      }
    }
    return baseState;
  }

  propagate(nextStates) {
    this.propagateCount++;
    // Perform each non-vc dependency.
    for (const idx in this.dependencies) {
      const dep = this.dependencies[idx];
      if (dep[0] === 'valueConstraint') continue;
      nextStates = this[dep[0]](nextStates, dep[1]);
    }
    return nextStates;
  }

  getValidDerivative(derivative, magnitude) {
    if (
      this.getSpaceLabel(magnitude) === 'max' && derivative === 1 ||
      this.getSpaceLabel(magnitude) === '0' && derivative === -1
    ) {
      return 0;
    }
    // Force the derivative to be a value between -1 and 1.
    return Math.min(1, Math.max(-1, derivative));
  }

  getValidDerivativeFromQuantity(quantityName, derivative, magnitude) {
    return this.state.quantities[quantityName].getValidDerivative(derivative, magnitude);
  }

  positiveInfluence(nextStates, [ quantityName, dependentQuantityName ]) {
    for (const idx in nextStates) {
      const nextState = nextStates[idx];
      if (nextState.quantities[quantityName].magnitude > 0) {
        const oldDerivative = nextState.quantities[dependentQuantityName].derivative;
        const derivative = nextState.quantities[dependentQuantityName].derivative;
        nextState.quantities[dependentQuantityName].derivative = this.getValidDerivativeFromQuantity(
          dependentQuantityName,
          derivative + 1,
          nextState.quantities[dependentQuantityName].magnitude,
        );
        nextState.log.push(`I+: ${quantityName} &#62; 0, so set ∂${dependentQuantityName} from ${
          this.state.quantities[dependentQuantityName].getDerivativeLabel(
            oldDerivative
          )
        } to ${
          this.state.quantities[dependentQuantityName].getDerivativeLabel(
            nextState.quantities[dependentQuantityName].derivative
          )
        }`);
      }
    }
    return this.state.quantities[dependentQuantityName].propagate(nextStates);
  }

  positiveProportional(nextStates, [ quantityName, dependentQuantityName ]) {
    for (const idx in nextStates) {
      const nextState = nextStates[idx];
      if (
        nextState.quantities[quantityName].derivative > 0 ||
        nextState.quantities[quantityName].derivative < 0
      ) {
        const increment = nextState.quantities[quantityName].derivative > 0 ? 1 : -1;
        const oldDerivative = nextState.quantities[dependentQuantityName].derivative;
        const derivative = nextState.quantities[dependentQuantityName].derivative;
        nextState.quantities[dependentQuantityName].derivative = this.getValidDerivativeFromQuantity(
          dependentQuantityName,
          derivative + increment,
          nextState.quantities[dependentQuantityName].magnitude,
        );
        const sign = increment === 1 ? '&#62;' : '&#60;';
        nextState.log.push(`P+: ∂${quantityName} ${sign} 0, so set ∂${dependentQuantityName} from ${
          this.state.quantities[dependentQuantityName].getDerivativeLabel(
            oldDerivative
          )
        } to ${
          this.state.quantities[dependentQuantityName].getDerivativeLabel(
            nextState.quantities[dependentQuantityName].derivative
          )
        }`);
      }
    }
    return this.state.quantities[dependentQuantityName].propagate(nextStates);
  }

  negativeInfluence(nextStates, [ quantityName, dependentQuantityName ]) {
    if (this.propagateCount > 1) {
      return this.capResult(nextStates);
    }
    const result = [ ...nextStates ];
    let changed = false;
    for (const idx in nextStates) {
      const nextState = nextStates[idx];
      const affectedDerivative = nextState.quantities[dependentQuantityName].derivative;
      const affectedDerivativeNew = this.getValidDerivativeFromQuantity(
        dependentQuantityName,
        affectedDerivative - 1,
        nextState.quantities[dependentQuantityName].magnitude,
      );
      // Only add another state if the magnitude of the originating quantity is greater than zero,
      // and the affected quantity derivative-1 is not the same as it already is,
      // e.g. in the case of an original derivative of -1, it will stay -1, so we do not add
      // another state.
      console.log(affectedDerivativeNew, affectedDerivative, affectedDerivativeNew !== affectedDerivative);
      if (
        nextState.quantities[quantityName].magnitude > 0 &&
        affectedDerivativeNew !== affectedDerivative
      ) {
        result.push({
          quantities: {
            ...nextState.quantities,
            [dependentQuantityName]: {
              ...nextState.quantities[dependentQuantityName],
              derivative: affectedDerivativeNew,
            },
          },
          log: [
            ...nextState.log,
            `I-: ${quantityName} &#62; 0, so set ∂${dependentQuantityName} from ${
              this.state.quantities[dependentQuantityName].getDerivativeLabel(
                affectedDerivative
              )
            } to ${
              this.state.quantities[dependentQuantityName].getDerivativeLabel(
                affectedDerivativeNew
              )
            }`,
          ],
        });
        changed = true;
      }
    }
    if (!changed) {
      return this.capResult(result);
    }
    return this.state.quantities[dependentQuantityName].propagate(result);
  }

  capResult(r) {
    const result = [...r];
    for (const idx in result) {
      for (const quantityName in result[idx].quantities) {
        result[idx].quantities[quantityName].derivative = this.getValidDerivativeFromQuantity(
          quantityName,
          result[idx].quantities[quantityName].derivative,
          result[idx].quantities[quantityName].magnitude,
        );
      }
    }
    return this.tapCurve(result);
  }

  tapCurve(r) {
    const result = [...r];
    for (const idx in r) {
      const state = r[idx];
      if (state.quantities['Inflow'].derivative === 1) {
        const copy = cloneDeep(state);
        copy.quantities['Inflow'].derivative = 0;
        result.push(copy);
      }
      // if (state.quantities['Inflow'].derivative === 0) {
      //   const copy = cloneDeep(state);
      //   copy.quantities['Inflow'].derivative = -1;
      //   result.push(copy);
      // }
    }
    return result;
  }

  /**
   * Returns the integer/numeric value for a given magnitude label.
   * @param {String} label 
   */
  getSpaceNumeric(label) {
    if (Number.isInteger(label)) {
      return label;
    }
    const idx = this.space.labels.findIndex((e) => e === label);
    return this.space.numeric[idx];
  }

  /**
   * Returns the string/label value for a given magnitude integer.
   */
  getSpaceLabel(numeric) {
    if (!Number.isInteger(numeric)) {
      return numeric;
    }
    const idx = this.space.numeric.findIndex((e) => e === numeric);
    return this.space.labels[idx];
  }

  getDerivativeLabel(numeric) {
    if (!Number.isInteger(numeric)) {
      return numeric;
    }
    const idx = DERIVATIVE_SPACE_INT.findIndex((e) => e === numeric);
    return DERIVATIVE_SPACE[idx];
  }
}

export default Quantity;
