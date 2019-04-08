const DERIVATIVE_SPACE = ['-', '0', '+'];
const DERIVATIVE_SPACE_INT = [-1, 0, 1];

class Quantity {
  /**
   * 
   * @param {Array} space 
   * @param {String} magnitude 
   * @param {String} derivative 
   */
  constructor({ space, magnitude, derivative, causation, dependencies = [] }) {
    this.space = space;
    this.magnitude = magnitude;
    this.derivative = derivative;
    this.causation = causation;
    this.dependencies = dependencies;
    this.nextStateFromLogicalConsequence = [];
    this.nextState = [];
    this.nextStateGeneratorCount = 0;
  }

  /**
   * 
   * @param {String} dependencyType 
   * @param {Array} dependencyInfo 
   */
  setDependency(dependencyType, dependencyInfo) {
    this.dependencies.push([dependencyType, dependencyInfo]);
  }

  logicalConsequence() {
    if (this.derivative === 0) {
      return;
    }
    // Find the index for the current derivative value in the numeric derivative space.
    const idx = DERIVATIVE_SPACE_INT.findIndex((e) => e === this.derivative);
    // Change the magnitude based on the derivative integer value, e.g. magnitude
    // of 'max' becomes '+' if derivative is '-' because derivative integer is -1.
    const magnitudeIdx = this.space.numeric.findIndex((e) => e === this.magnitude);
    const magnitude = this.space.numeric[magnitudeIdx + this.derivative];
    // Add the base state, and also add a state for each possible alteration of the
    // derivative.
    for (let i = -1; i <= 1; i++) {
      const derivativeIdx = idx + i;
      // We cannot add a possible state if there doesn't exist a neighboring
      // derivative, e.g. in the case of '-1' no left neighbor in the space.
      if (derivativeIdx < 0 || derivativeIdx > (DERIVATIVE_SPACE.length - 1)) {
        continue;
      }
      this.nextStateFromLogicalConsequence.push({
        magnitude,
        'derivative': DERIVATIVE_SPACE_INT[derivativeIdx],
        space: this.space,
        dependencies: this.dependencies,
        causation,
      });
    }
  }

  propagate() {
    // Perform each dependency.
    console.log('PROPAGATE');
    for (const idx in this.dependencies) {
      console.log('depdency idx ' + idx);
      const dep = this.dependencies[idx];
      console.log(dep[0], dep[1]);
      this[dep[0]](...dep[1]);
    }
    return;
  }

  /**
   * Receives a new value for a given type from a different Quantity caused
   * by a dependency.
   * @param {String} type either 'derivative' or 'magnitude'
   * @param {String} value the value for the type
   * @param {Object} causation an Object containing the cause of this state change
   */
  receive(type, value, causation) {
    const isDerivative = type === 'derivative';
    const otherType = isDerivative ? 'magnitude' : 'derivative';
    console.log('RECEIVED THE CALLLL');
    this.nextState.push({
      // In the case of a derivative change, increment the current derivative with
      // the given value. In the case of a magnitude, set the current magnitude
      // to the given value.
      [type]: isDerivative ? Math.max(-1, Math.min(1, this.derivative+value)) : value,
      [otherType]: this[otherType],
      space: this.space,
      dependencies: this.dependencies,
      causation,
    });
  }

  getNextState() {
    console.log(this.nextState);
    // if (this.nextState.length === 0) {
    //   this.nextState = this.nextStateFromLogicalConsequence;
    // }
    // If there does not exist any changed next state, we return
    // the original unchanged state.
    if (this.nextState.length === 0) {
      return {
        'magnitude': this.magnitude, 'derivative': this.derivative,
        'space': this.space, 'dependencies': this.dependencies,
      };
    }
    // Otherwise, we return the next state.
    // console.log(this.nextState[this.nextStateGeneratorCount]);
    return this.nextState[this.nextStateGeneratorCount++];
  }

  positiveInfluence([ quantityName, dependentQuantityName ]) {
    console.log('POSITIVE INFLUENCE CALLED');
    // If the quantity magnitude is greater than zero,
    // we send the derivative to the depedent quantity.
    if (this.magnitude > 0) {
      this.state.quantities[dependentQuantityName].receive('derivative', 1, {
        cause: 'positiveInfluence',
        quantityName,
        dependentQuantityName,
      });
    }
  }

  negativeInfluence([ quantityName, dependentQuantityName ]) {
    // If the quantity magnitude is greater than zero,
    // we send the derivative to the depedent quantity.
    if (this.magnitude > 0) {
      this.state.quantities[dependentQuantityName].receive('derivative', -1, {
        cause: 'negativeInfluence',
        quantityName,
        dependentQuantityName,
      });
    }
  }

  positiveProportional([ quantityName, dependentQuantityName ]) {
    if (this.derivative > 0) {
      this.state.quantities[dependentQuantityName].receive('derivative', 1, {
        cause: 'positiveProportional',
        quantityName,
        dependentQuantityName,
      });
    }
  }

  valueConstraint([ quantityName, quantityValue, dependentQuantityName, dependentValue ]) {
    if (this.magnitude === this.getSpaceNumeric(quantityValue)) {
      this.state.quantities[dependentQuantityName].receive('magnitude', this.getSpaceNumeric(dependentValue), {
        cause: 'valueConstraint',
        quantityName,
        quantityValue,
        dependentQuantityName,
        dependentValue,
      });
    }
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
}

export default Quantity;
