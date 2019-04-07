class State {
  /**
   * 
   * @param {Array} value 
   * @param {Array} childIds
   */
  constructor(value) {
    this.value = value;
  }

  /**
   * @returns unique state id
   */
  id() {
    return this.id;
  }

  /**
   * 
   * @param {Number} childId 
   */
  addChildConnection(childId) {
    this.childIds.append(childId);
  }

  /**
   * Checks if the state triggers one of their relations.
   * From each triggered relation it calculates the next possible state(s).
   * @returns a list of stateValues
   */
  next() {
    return [];
  }
}

export default State;
