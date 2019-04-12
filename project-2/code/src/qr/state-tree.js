class StateTree {
  /**
   * 
   * @param {Object} states id-state mapping 
   */
  constructor(state) {
    this.states = {};
    this.stringStates = {};
    if (state) {
      this.addState(state);
    }
  }

  /**
   * 
   * @param {State} state 
   */
  addState(state) {
    this.states[state.id] = state;
    this.stringStates[this.toStringState(state)] = state.id;
  }

  /**
   * 
   * @param {*} state can be either an Object of state value or a State instance
   */
  toStringState(state) {
    let text = '';
    const quantities = state.quantities;
    for (const name in quantities) {
      text += `m:${quantities[name].magnitude},d:${quantities[name].derivative};`;
    }
    return text;
  }

  /**
   * Tries to find the given state within the current state tree.
   * If found, it returns the ID for the found state, otherwise, returns -1.
   * @param {*} state can be either an Object of state value or a State instance
   */
  findStateId(state) {
    const targetStateString = this.toStringState(state);
    const foundStateId = this.stringStates[targetStateString];
    return (foundStateId >= 0) ? foundStateId : -1;
  }
  
  /**
   * 
   * @param {Number} parentId 
   * @param {Array} child tuple containing childId and log 
   */
  addConnection(parentId, [childId, log]) {
    if (parentId === childId) return;
    this.states[parentId].addChildConnection([childId, log]);
  }
}

export default StateTree;
