class State {
  /**
   * 
   * @param {Number} id
   * @param {Array} value
   */
  constructor(id, quantities, log) {
    this.id = id;
    this.quantities = quantities;
    this.children = [];
    this.log = this.cleanLogs(log);
    for (const q in this.quantities) {
      this.quantities[q].state = this;
    }
  }

  cleanLogs(log) {
    const newLog = [];
    let prevl;
    for (const idx in log) {
      const l = log[idx];
      if (l === prevl) continue;
      newLog.push(l);
      prevl = l;
    }
    return newLog;
  }

  /**
   * @returns unique state id
   */
  getId() {
    return this.id;
  }

  /**
   * 
   * @param {Array} child 
   */
  addChildConnection([childId, log]) {
    this.children.push([childId, this.cleanLogs(log)]);
  }

  /**
   * 
   * @param {String} name the name of the quantity
   * @param {String} type either 'magnitude' or 'derivative'
   * @param {String} value new value for the quantity type
   */
  setNextQuantityState(quantityName, quantityType, quantityValue) {
    this.quantities[quantityName].enableDerivative();
  }

  /**
   * 
   * @param {String} dependencyType 
   * @param {String} quantityName 
   * @param  {...String} dependencyInfo 
   */
  setQuantityDependency(dependencyType, quantityName, ...dependencyInfo) {
    this.quantities[quantityName].setDependency(dependencyType, [quantityName, ...dependencyInfo]);
  }

  /**
   * Generates next states for the current state, by first checking for
   * logical consequences from the current derivative values for each quantity.
   * Then each quantity communicates its values to their dependencies, from
   * which each dependent quantity sets their new possible value(s).
   * Lastly, we iterate through all quantities and create states from all possible
   * combinations of values within the quantities.
   * @returns a list of stateValues
   */
  next() {
    // First check all quantities for logical consequences from the current quantity
    // state values, e.g. setting magnitude from '0' to '+' if derivative is '+'.
    let baseState = { quantities: {}, log: [] };
    for (const quantityName in this.quantities) {
      const [status, log] = this.quantities[quantityName].processDerivative(quantityName);
      baseState.quantities[quantityName] = status;
      if (log) {
        baseState.log.push(log);
      }
    }
    // Perform value constraints.
    let baseStateAfterVC = { ...baseState };
    for (const quantityName in baseState.quantities) {
      baseStateAfterVC = this.quantities[quantityName].valueConstraint(baseStateAfterVC);
    }
    // Second, propagate the state value changes from performing the logical
    // consequences through the entire system.
    const nextStates = this.quantities['Inflow'].propagate([{ ...baseStateAfterVC }]);
    // Generate an Array of { state, parentId } pairs.
    const result = [];
    for (const idx in nextStates) {
      result.push({ stateValue: nextStates[idx], parentId: this.id });
    }
    return result;
  }
}

export default State;
