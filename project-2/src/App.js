import React, { Component } from 'react';
import Graph from './Graph.js';
import './App.css';

class App extends Component {

  proportionalplus(state, a, b) {
    let qad = state.quantities[a].derivative;
    let qbd = state.quantities[b].derivative;
    if (qad === '+') {
      if (qbd === '-')
          state.quantities[b] = '0';
      if (qbd === '0') //TODO: check if this needs elseif
          state.quantities[b] = '+'
    }
    if (qad === '-') {
      if (qbd === '+')
        state.quantities[b] = '0';
      if (qbd === '0') //TODO: check if this needs elseif
        state.quantities[b] = '-'
    }
    return state;
  }

  valueconstraint(state, a, b) {
    if (state.quantities[a].quantity === 'max')
      state.quantities[b].quantity = 'max';
    if (state.quantities[a].quantity === '0')
      state.quantities[b].quantity = '0';
    return state;
  } 

  influence(state, a, b, c) {
    let qa = state.quantities[a];
    let qb = state.quantities[b];
    let qc = state.quantities[c];

    if (qa.quantity === '0' && qb.quantity === '0')
      return [state];

    if (qa.quantity === '0' && (qb.quantity === '+' || qb.quantity == 'max')) {
      if (qc.derivative === '0')
        state.quantities[c].derivative = '-';
      if (qc.derivative === '+')
        state.quantities[c].derivative = '0';
      return [state];
    }

    if (qa.quantity === '+' && qb.quantity === '0') {
      if (qc.derivative === '0')
        state.quantities[c].derivative = '+';
      if (qc.derivative === '-')
        state.quantities[c].derivative = '0';
    }

    if (qa.quantity === '+' && (qb.quantity === '+' || qb.quantity === 'max')) {
      if (qc.derivative === '-') {
        let newState = state.copy();
        newState.quantities[c].derivative = '0';
        return [newState, state];
      }
      if (qc.derivative === '0') {
        let newState1 = state.copy();
        newState1.quantities[c].derivative = '0';
        let newState2 = state.copy();
        newState2.quantities[c].derivative = '+';
        return [newState1, newState2, state];
      }
      if (qc.derivative === '+') {
        let newState = state.copy();
        newState.quantities[c].derivative = '0';
        return [newState, state];
      }
    }
  }

  derivative(state) {
    let newState = state.copy();
    for (const [name, q] in Object.entries(state.quantities)) {
      let index = q.space.indexOf(q.quantity);
      if (q.derivative === '+' && (index + 1) != q.space.length)
        newState.quantities[name].quantity = q.space[index+1];
      if (q.derivative === '-' && (index != 0))
        newState.quantities[name].quantity = q.space[index - 1];
    }
    return newState;
  }

  derivative2(state) {
    let newState = state.copy();
    for (var key in state.quantities) {
      if (newState.quantities[key].quantity === 'max' && newState.quantities[key].derivative == '+')
        newState.quantities.derivative = '0';
      if (newState.quantities[key].quantity === '0' && newState.quantities[key].derivative === '-') {
        newState.quantities.derivative = '0';
      }
    }
    return newState;
  }
  
  tap_curve(state) {
    let newState = state.copy();
    if (state.quantities['inlfow'].derivative == '+') {
      newState.quantities['inflow'].derivative = '0';
      return [newState, state];
    }
    if (state.quantities['inflow'].derivative == '0') {
      newState.quantities['inflow'].derivative = '-';
      return [newState, state];
    }
    return state;
  }

  inferStates(state) {
    state = this.derivative(state);
    state = this.vc(state, 'volume', 'outflow');
    let next_states = this.influence(state, 'inflow', 'outflow', 'volume');
    next_states = next_states.map(s => this.proportionalplus(s, 'volume', 'outflow'));
    next_states = next_states.map(s => this.tap_curve(s)); //TODO flatten this list?
    next_states = next_states.map(s => this.derivative2(s)); 
  }

  to_short_string(state) {
    return Object.keys(state.quantities).map(s => state.quantities[s].quantity + state.quantities[s].derivative).join();
  }

  search(tree) {
    let to_do = tree.leaf_nodes;
    let prop = this.to_short_string(to_do[0]['state']);
    let visited = {};
    visited[prop] = {'number': to_do[0]['number'], 'children': []};

    while (to_do) { //TODO: does this work?
      let node = to_do.pop();
      let next_states = this.inferStates(node['state']);
      node['children'] = [];
      for (var i = 0; i < next_states.state.length; i++) {
        let state = next_states[i];
        if (!(this.to_short_string(state) in visited)) {
          this.number_of_nodes += 1;
          let new_node = {'state': state, 'number': this.number_of_nodes}
          visited[this.to_short_string(state)] = {'number': this.number_of_nodes, 'children': []};
          to_do.append(new_node);
        }
        visited[this.to_short_string(node['state'])]['children'].push(this.to_short_string(state));
        //node['children'].append(new_node); TODO: check this
      }
    }
    return visited;
  }

  componentDidMount() {
    let system = new State();
    system.turn_on_tap();
    let tree = new Tree(system);
    let result = this.search(tree);
    console.log(result);
  }

  render() {
    return (
      <div className="App">
        {/* <Graph></Graph> */}
        
      </div>
    );
  }
}

export default App;

class Quantity {//} extends Component {
  constructor(props) {
    this.quantity = '0';
    this.derivative = '0';
    this.space = props;
  }
}

class State {//extends Component {
    constructor() {
      let outflow = new Quantity(['0', '+', 'max']);
      let volume = new Quantity(['0', '+', 'max']);
      let inflow = new Quantity(['0', '+']);
      this.quantities = {outflow, volume, inflow};
    }

    turn_on_tap() {
      //TODO: check if this works
      this.quantities['inflow'].derivative = '+';
    }

    copy() {
      //TODO: check if this works
      return JSON.parse(JSON.stringify(this));
    }
}

class Tree {//extends Component {
  constructor(props) {
    this.number_of_nodes = 1;
    this.root = {'state': props, 'number': 1}; //TODO check if props contains the expected contents
    this.leaf_nodes = [this.root];
  }
}