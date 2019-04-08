import isEqual from 'lodash/isEqual';

class State {
  /**
   * 
   * @param {Number} id
   * @param {Array} value
   */
  constructor(id, quantities) {
    this.id = id;
    this.quantities = quantities;
    this.childIds = [];
    for (const q in this.quantities) {
      this.quantities[q].state = this;
    }
  }

  /**
   * @returns unique state id
   */
  getId() {
    return this.id;
  }

  /**
   * 
   * @param {Number} childId 
   */
  addChildConnection(childId) {
    this.childIds.push(childId);
  }

  /**
   * 
   * @param {String} name the name of the quantity
   * @param {String} type either 'magnitude' or 'derivative'
   * @param {String} value new value for the quantity type
   */
  setNextQuantityState(quantityName, quantityType, quantityValue) {
    this.quantities[quantityName].receive(quantityType, quantityValue, {
      cause: 'exogenousAction',
      quantityName,
      quantityType,
      quantityValue,
    });
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
   * Retrieves the next quantity state for each quantity.
   * @returns an Object of quantityName-states pairs
   */
  // getNextQuantityStatesMap() {
  //   const quantityStates = {};
  //   for (const quantityName in this.quantities) {
  //     if (!(quantityName in quantityStates)) {
  //       quantityStates[quantityName] = [];
  //     }
  //     quantityStates[quantityName].push(this.quantities[quantityName].getNextStates());
  //   }
  //   return quantityStates;
  // }

  /**
   * @returns an Array of Objects, each Object containing values for a
   * full potential State
   */
  generateNextStateCombinations() {
    const nextStates = [];
    while (true) {
      const nextState = {};
      for (const quantityName in this.quantities) {
        console.log('get next state for ' + quantityName);
        nextState[quantityName] = this.quantities[quantityName].getNextState();
      }
      // If the new state is equal to the previous generated state,
      // it means that all possible combinations are generated, so
      // we stop the state combination generation.
      if (isEqual(nextState, nextStates[nextStates.length - 1])) {
        break;
      }
      nextStates.push([nextState, [this.id]]);
      break;
    }
    this.nextStates = nextStates;
    console.log(nextStates);
    return nextStates;
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
    for (const quantityName in this.quantities) {
      this.quantities[quantityName].logicalConsequence();
    }
    console.log(this.quantities);
    // Second, propagate the state value changes from performing the logical
    // consequences through the entire system.
    this.quantities['Inflow'].propagate();
    // Lastly, get the combinations of quantities.
    return this.generateNextStateCombinations();
  }
}

export default State;
