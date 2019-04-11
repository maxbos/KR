class TreeLevel {
  /**
   * 
   * @param {Array} stateValues Array of Objects containing (1) state value object and (2) parent state ID
   */
  constructor(stateValues = []) {
    this.stateValues = stateValues;
  }

  /**
   * For each state value that already exists, merge the connections.
   * @param {Array} stateValues Array of Objects containing (1) state value object and (2) parent state ID
   */
  add(stateValues) {
    this.stateValues = [
      ...this.stateValues,
      ...stateValues,
    ];
  }
  
  /**
   * @returns stateValues
   */
  get() {
    return this.stateValues;
  }

  /**
   * @returns whether the tree level contains state values
   */
  isEmpty() {
    return this.stateValues.length === 0;
  }
}

export default TreeLevel;
