class StateTree {
  /**
   * 
   * @param {Object} states id-state mapping 
   */
  constructor(states = {}) {
    this.states = states;
  }

  /**
   * 
   * @param {State} state 
   */
  addState(state) {
    this.states[state.id()] = state;
  }
  
  /**
   * 
   * @param {Number} parentId 
   * @param {Number} childId 
   */
  addConnection(parentId, childId) {
    this.states[parentId].addChildConnection(childId);
  }

  /**
   * Add the ID of a given child state to each parent state
   * of a given list of parent state IDs.
   * @param {Array} parentIds array of numbers
   * @param {Number} childId 
   */
  addConnections(parentIds, childId) {
    for (const parentId in parentIds) {
      this.addConnection(parentId, childId);
    }
  }
}

export default StateTree;
