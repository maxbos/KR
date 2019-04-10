import { Quantity, State, StateTree, TreeLevel } from '.';

const INFLOW = {
  space: { labels: ['0', '+'], numeric: [0, 1] },
  magnitude: 0, derivative: 0,
};
const VOLUME = {
  space: { labels: ['0', '+', 'max'], numeric: [0, 1, 2] },
  magnitude: 0, derivative: 0,
};
const OUTFLOW = {
  space: { labels: ['0', '+', 'max'], numeric: [0, 1, 2] },
  magnitude: 0, derivative: 0,
};

class World {
  constructor() {
    this.stateCount = -1;
    this.stateTree = new StateTree();
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
    const quantities = {};
    for (const quantityName in stateValue.quantities) {
      quantities[quantityName] = new Quantity(stateValue.quantities[quantityName]);
    }
    return new State(this.nextId(), quantities, stateValue.log);
  }

  setupInitialState() {
    const state = this.newState({
      quantities: {
        'Inflow': INFLOW,
        'Volume': VOLUME,
        'Outflow': OUTFLOW,
      },
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
    this.stateTree.addState(rootState);
    rootState.setNextQuantityState('Inflow', 'derivative', 1);
    let currentTreeLevel = new TreeLevel(rootState.next());
    let count = 0;
    while (!currentTreeLevel.isEmpty()) {
      const nextTreeLevel = new TreeLevel();
      const currentTree = currentTreeLevel.get();
      for (const idx in currentTree) {
        const { stateValue, parentId } = currentTree[idx];
        // If the state tree does not yet contain this state value,
        // we want to add this state,
        let stateId = this.stateTree.findStateId(stateValue);
        if (stateId === -1) {
          const state = this.newState(stateValue);
          stateId = state.id;
          // Add the current state as a free node (not yet any connections) to
          // the state-tree.
          this.stateTree.addState(state);
          // Merge the state values that follow from the current state into the
          // next tree level.
          nextTreeLevel.add(state.next());
        }
        // Add the parent-child connection to each parent state that is included
        // in the `parentIds` list.
        this.stateTree.addConnection(parentId, stateId);
        count++;
        // if (count > 3) break;
      }
      // if (count > 3) break;
      currentTreeLevel = nextTreeLevel;
    }
  }

  /**
   * @returns the current state tree
   */
  getStateTree() {
    return this.stateTree;
  }
}

export default World;
