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
    // this.nextStateFromLogicalConsequence = [];
    // this.nextState = [];
    // this.nextStateGeneratorCount = 0;
  }

  /**
   * 
   * @param {String} dependencyType 
   * @param {Array} dependencyInfo 
   */
  setDependency(dependencyType, dependencyInfo) {
    this.dependencies.push([dependencyType, dependencyInfo]);
  }

  // logicalConsequence() {
  //   if (this.derivative === 0) {
  //     return;
  //   }
  //   // Find the index for the current derivative value in the numeric derivative space.
  //   const idx = DERIVATIVE_SPACE_INT.findIndex((e) => e === this.derivative);
  //   // Change the magnitude based on the derivative integer value, e.g. magnitude
  //   // of 'max' becomes '+' if derivative is '-' because derivative integer is -1.
  //   const magnitudeIdx = this.space.numeric.findIndex((e) => e === this.magnitude);
  //   const magnitude = this.space.numeric[magnitudeIdx + this.derivative];
  //   // Add the base state, and also add a state for each possible alteration of the
  //   // derivative.
  //   for (let i = -1; i <= 1; i++) {
  //     const derivativeIdx = idx + i;
  //     // We cannot add a possible state if there doesn't exist a neighboring
  //     // derivative, e.g. in the case of '-1' no left neighbor in the space.
  //     if (derivativeIdx < 0 || derivativeIdx > (DERIVATIVE_SPACE.length - 1)) {
  //       continue;
  //     }
  //     this.nextStateFromLogicalConsequence.push({
  //       magnitude,
  //       'derivative': DERIVATIVE_SPACE_INT[derivativeIdx],
  //       space: this.space,
  //       dependencies: this.dependencies,
  //       causation,
  //     });
  //   }
  // }

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
    let magnitude = this.magnitude;
    console.log('processderivative', name, 'current magnitude', this.magnitude);
    if (this.derivative !== 0) {
      // Change the magnitude based on the derivative integer value, e.g. magnitude
      // of 'max' becomes '+' if derivative is '-' because derivative integer is -1.
      const magnitudeIdx = this.space.numeric.findIndex((e) => e === this.magnitude);
      const newMagnitudeIdx = Math.max(0, Math.min(this.space.numeric.length-1, magnitudeIdx + this.derivative));
      magnitude = this.space.numeric[newMagnitudeIdx];
      console.log(magnitudeIdx, 'new magnitude idx', newMagnitudeIdx, magnitude);
    }
    return [{
      magnitude,
      derivative: this.derivative,
      space: this.space,
      dependencies: this.dependencies,
    }, `changed magnitude of ${name} based on derivative`];
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
              `performed value constraint, set ${dependentQuantityName} to ${dependentValue} because ${quantityName} is ${quantityValue}`,
            ],
          };
        }
      }
    }
    return baseState;
  }

  propagate(nextStates) {
    console.log('propagate', nextStates);
    // Perform each non-vc dependency.
    for (const idx in this.dependencies) {
      const dep = this.dependencies[idx];
      if (dep[0] === 'valueConstraint') continue;
      console.log('propagate dependency', dep);
      nextStates = this[dep[0]](nextStates, dep[1]);
    }
    return nextStates;
  }

  getValidDerivative(derivative) {
    // Force the derivative to be a value between -1 and 1.
    return Math.min(1, Math.max(-1, derivative));
  }

  positiveInfluence(nextStates, [ quantityName, dependentQuantityName ]) {
    for (const idx in nextStates) {
      const nextState = nextStates[idx];
      if (nextState.quantities[quantityName].magnitude > 0) {
        const derivative = nextState.quantities[dependentQuantityName].derivative;
        nextState.quantities[dependentQuantityName].derivative = this.getValidDerivative(derivative + 1);
        nextState.log.push(`positive influence from ${quantityName} to ${dependentQuantityName}`);
      }
    }
    return this.state.quantities[dependentQuantityName].propagate(nextStates);
  }

  positiveProportional(nextStates, [ quantityName, dependentQuantityName ]) {
    for (const idx in nextStates) {
      const nextState = nextStates[idx];
      if (nextState.quantities[quantityName].derivative > 0) {
        const derivative = nextState.quantities[dependentQuantityName].derivative;
        nextState.quantities[dependentQuantityName].derivative = this.getValidDerivative(derivative + 1);
        nextState.log.push(`positive proportional from ${quantityName} to ${dependentQuantityName}`);
      }
    }
    return nextStates;
  }

  negativeInfluence(nextStates, [ quantityName, dependentQuantityName ]) {
    const result = nextStates;
    for (const idx in nextStates) {
      const nextState = nextStates[idx];
      const affectedDerivative = nextState.quantities[dependentQuantityName].derivative;
      // Only add another state if the magnitude of the originating quantity is greater than zero,
      // and the affected quantity derivative-1 is not the same as it already is,
      // e.g. in the case of an original derivative of -1, it will stay -1, so we do not add
      // another state.
      if (
        nextState.quantities[quantityName].magnitude > 0 &&
        this.getValidDerivative(affectedDerivative - 1) !== affectedDerivative
      ) {
        result.push({
          quantities: {
            ...nextState.quantities,
            [dependentQuantityName]: {
              ...nextState.quantities[dependentQuantityName],
              derivative: this.getValidDerivative(affectedDerivative - 1),
            },
          },
          log: [
            ...nextState.log,
            `negative influence from ${quantityName} to ${dependentQuantityName}`,
          ],
        });
      }
    }
    return nextStates;
  }

  // propagate() {
  //   // Perform each dependency.
  //   console.log('PROPAGATE');
  //   for (const idx in this.dependencies) {
  //     console.log('depdency idx ' + idx);
  //     const dep = this.dependencies[idx];
  //     console.log(dep[0], dep[1]);
  //     this[dep[0]](...dep[1]);
  //   }
  //   return;
  // }

  /**
   * Receives a new value for a given type from a different Quantity caused
   * by a dependency.
   * @param {String} type either 'derivative' or 'magnitude'
   * @param {String} value the value for the type
   * @param {Object} causation an Object containing the cause of this state change
   */
  // receive(type, value, causation) {
  //   const isDerivative = type === 'derivative';
  //   const otherType = isDerivative ? 'magnitude' : 'derivative';
  //   console.log('RECEIVED THE CALLLL');
  //   this.nextState.push({
  //     // In the case of a derivative change, increment the current derivative with
  //     // the given value. In the case of a magnitude, set the current magnitude
  //     // to the given value.
  //     [type]: isDerivative ? Math.max(-1, Math.min(1, this.derivative+value)) : value,
  //     [otherType]: this[otherType],
  //     space: this.space,
  //     dependencies: this.dependencies,
  //     causation,
  //   });
  // }

  // getNextState() {
  //   console.log(this.nextState);
  //   // if (this.nextState.length === 0) {
  //   //   this.nextState = this.nextStateFromLogicalConsequence;
  //   // }
  //   // If there does not exist any changed next state, we return
  //   // the original unchanged state.
  //   if (this.nextState.length === 0) {
  //     return {
  //       'magnitude': this.magnitude, 'derivative': this.derivative,
  //       'space': this.space, 'dependencies': this.dependencies,
  //     };
  //   }
  //   // Otherwise, we return the next state.
  //   // console.log(this.nextState[this.nextStateGeneratorCount]);
  //   return this.nextState[this.nextStateGeneratorCount++];
  // }

  // positiveInfluence([ quantityName, dependentQuantityName ]) {
  //   console.log('POSITIVE INFLUENCE CALLED');
  //   // If the quantity magnitude is greater than zero,
  //   // we send the derivative to the depedent quantity.
  //   if (this.magnitude > 0) {
  //     this.state.quantities[dependentQuantityName].receive('derivative', 1, {
  //       cause: 'positiveInfluence',
  //       quantityName,
  //       dependentQuantityName,
  //     });
  //   }
  // }

  // negativeInfluence([ quantityName, dependentQuantityName ]) {
  //   // If the quantity magnitude is greater than zero,
  //   // we send the derivative to the depedent quantity.
  //   if (this.magnitude > 0) {
  //     this.state.quantities[dependentQuantityName].receive('derivative', -1, {
  //       cause: 'negativeInfluence',
  //       quantityName,
  //       dependentQuantityName,
  //     });
  //   }
  // }

  // positiveProportional([ quantityName, dependentQuantityName ]) {
  //   if (this.derivative > 0) {
  //     this.state.quantities[dependentQuantityName].receive('derivative', 1, {
  //       cause: 'positiveProportional',
  //       quantityName,
  //       dependentQuantityName,
  //     });
  //   }
  // }

  // valueConstraint([ quantityName, quantityValue, dependentQuantityName, dependentValue ]) {
  //   if (this.magnitude === this.getSpaceNumeric(quantityValue)) {
  //     this.state.quantities[dependentQuantityName].receive('magnitude', this.getSpaceNumeric(dependentValue), {
  //       cause: 'valueConstraint',
  //       quantityName,
  //       quantityValue,
  //       dependentQuantityName,
  //       dependentValue,
  //     });
  //   }
  // }

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
}

export default Quantity;
