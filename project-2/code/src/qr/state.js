class State {
  /**
   * 
   * @param {Number} id
   * @param {Array} value
   */
  constructor(id, quantities) {
    this.id = id;
    this.quantities = quantities;
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
   * 
   * @param {String} name the name of the quantity
   * @param {String} type either 'magnitude' or 'derivative'
   * @param {String} value new value for the quantity type
   */
  setQuantity(name, type, value) {
    this.quantities[name].set(type, value);
  }

  /**
   * 
   * @param {String} dependencyType 
   * @param {String} quantityName 
   * @param  {...String} dependencyInfo 
   */
  setQuantityDependency(dependencyType, quantityName, ...dependencyInfo) {
    this.quantities[quantityName].setDependency(dependencyType, ...dependencyInfo, this.quantities);
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
    return [];
  }
}

export default State;
