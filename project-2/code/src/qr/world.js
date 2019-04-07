import { State, StateTree, TreeLevel } from '.';

const ROOT_STATE_VALUE = '0, 0, 0, 0';

class World {
  constructor() {
    this.init();
    this.stateCount = 0;
  }

  /**
   * @returns the next unique state ID
   */
  nextId() {
    return ++this.stateCount;
  }

  init() {
    const stateTree = new StateTree();
    const rootState = new State(ROOT_STATE_VALUE);
    let currentTreeLevel = new TreeLevel(rootState.next());
    while (!currentTreeLevel.isEmpty()) {
      const nextTreeLevel = new TreeLevel();
      for (const stateValue in currentTreeLevel.get()) {
        const state = new State(stateValue);
        // Add the current state as a free node (not yet any connections) to
        // the state-tree.
        stateTree.addState(state);
        // Get the IDs for the states from which the current state originated.
        const parentIds = currentTreeLevel.getStateValueParents(stateValue);
        // Add the parent-child connection to each parent state that is included
        // in the `parentIds` list.
        stateTree.addConnections(parentIds, state.id());
        // Merge the state values that follow from the current state into the
        // next tree level.
        nextTreeLevel.add(state.next());
      }
      currentTreeLevel = nextTreeLevel;
    }
  }
}

export default World;
