import { Quantity, State, StateTree, TreeLevel } from '.';

class World {
  constructor() {
    this.stateCount = -1;
    this.init();
  }

  /**
   * @returns the next unique state ID
   */
  nextId() {
    return ++this.stateCount;
  }

  /**
   * 
   * @param {String} stateValue 
   */
  newState(stateValue) {
    return new State(this.nextId(), stateValue);
  }

  setupInitialState() {
    const state = this.newState({
      'Inflow': new Quantity(['0', '+'], '0', '0'),
      'Volume': new Quantity(['0', '+', 'max'], '0', '0'),
      'Outflow': new Quantity(['0', '+', 'max'], '0', '0'),
    });
    state.setQuantityDependency('positiveInfluence', 'Inflow', 'Volume');
    state.setQuantityDependency('negativeInfluence', 'Outflow', 'Volume');
    state.setQuantityDependency('positiveProportional', 'Volume', 'Outflow');
    state.setQuantityDependency('valueConstraint', 'Volume', 'max', 'Outflow', 'max');
    state.setQuantityDependency('valueConstraint', 'Volume', '0', 'Outflow', '0');
    return state;
  }

  init() {
    const rootState = this.setupInitialState();
    const stateTree = new StateTree(rootState);
    rootState.setQuantity('Inflow', 'derivative', '+');
    let currentTreeLevel = new TreeLevel(rootState.next());
    while (!currentTreeLevel.isEmpty()) {
      const nextTreeLevel = new TreeLevel();
      for (const [stateValue, parentIds] in currentTreeLevel.get()) {
        const state = this.newState(stateValue);
        // Add the current state as a free node (not yet any connections) to
        // the state-tree.
        stateTree.addState(state);
        // Get the IDs for the states from which the current state originated.
        // const parentIds = currentTreeLevel.getStateValueParents(stateValue);
        // Add the parent-child connection to each parent state that is included
        // in the `parentIds` list.
        stateTree.addConnections(parentIds, state.id());
        // Merge the state values that follow from the current state into the
        // next tree level.
        nextTreeLevel.add(state.next());
      }
      currentTreeLevel = nextTreeLevel;
    }
    this.stateTree = stateTree;
  }
}

export default World;
