import merge from 'lodash/merge';

class TreeLevel {
  /**
   * 
   * @param {Array} stateValues mapping of stateValues and parent State IDs
   */
  constructor(stateValues) {
    this.stateValues = stateValues;
  }

  /**
   * For each state value that already exists, merge the connections.
   * @param {Array} stateValues mapping of stateValues and parent State IDs
   */
  add(stateValues) {
    this.stateValues = merge(this.stateValues, stateValues);
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
    return !Object.keys(this.stateValues).length;
  }
}

export default TreeLevel;
