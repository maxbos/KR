const DERIVATIVE_SPACE = ['-', '0', '+'];
const DERIVATIVE_SPACE_INT = [-1, 0, 1];

class Quantity {
  /**
   * 
   * @param {Array} space 
   * @param {String} magnitude 
   * @param {String} derivative 
   */
  constructor({ space, magnitude, derivative }) {
    this.space = space;
    this.magnitude = magnitude;
    this.derivative = derivative;
    this.dependencies = [];
    this.nextStateFromLogicalConsequence = [];
    this.nextState = [];
    this.nextStateGeneratorCount = 0;
  }

  setDependency(dependencyType, dependencyInfo, quantities) {
    this.dependencies.push([dependencyType, dependencyInfo, quantities]);
  }

  logicalConsequence() {
    const idx = DERIVATIVE_SPACE.findIndex((e) => e === this.derivative);
    // Change the magnitude based on the derivative integer value, e.g. magnitude
    // of 'max' becomes '+' if derivative is '-' because derivative integer is -1.
    const magnitudeIdx = this.space.findIndex((e) => e === this.magnitude);
    const magnitude = this.space[magnitudeIdx + DERIVATIVE_SPACE_INT[idx]];
    // Add the base state, and also add a state for each possible alteration of the
    // derivative.
    for (let i = -1; i++; i <= 1) {
      const derivativeIdx = idx - i;
      // We cannot add a possible state if there doesn't exist a neighboring
      // derivative, e.g. in the case of '-1' no left neighbor in the space.
      if (derivativeIdx < 0 || derivativeIdx > (DERIVATIVE_SPACE.length - 1)) {
        continue;
      }
      this.nextStateFromLogicalConsequence.push([{
        magnitude,
        'derivative': DERIVATIVE_SPACE[derivativeIdx],
      }]);
    }
  }

  propagate() {
    return [];
  }

  /**
   * Receives a new value for a given type from a different Quantity caused
   * by a dependency.
   * @param {String} type either 'derivative' or 'magnitude'
   * @param {String} value the value for the type
   * @param {Object} causation an Object containing the cause of this state change
   */
  receive(type, value, causation) {
    const otherType = type === 'derivative' ? 'magnitude' : 'derivative';
    this.nextState.push({
      [type]: value,
      [otherType]: this[otherType],
      space: this.space,
      causation,
    });
  }

  getNextState() {
    // If there does not exist any changed next state, we return
    // the original unchanged state.
    if (this.nextState.length === 0) {
      return { 'magnitude': this.magnitude, 'derivative': this.derivative, 'space': this.space, };
    }
    // Otherwise, we return the next state.
    return this.nextState[this.nextStateGeneratorCount++];
  }

  positiveInfluence(dependentQuantityName, quantities) {

  }

  negativeInfluence(dependentQuantityName, quantities) {

  }

  positiveProportional(dependentQuantityName, quantities) {
    
  }

  valueConstraint(value, dependentQuantityName, dependentValue, quantities) {

  }
}

export default Quantity;
