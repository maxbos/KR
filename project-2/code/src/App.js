import React, { Component } from 'react';
import { graphviz} from 'd3-graphviz';
import World from './qr/world.js';
import * as d3 from "d3";
import './App.css';

class App extends Component {  
  constructor(props) {
    super(props);
    this.state = {
      states: new World().stateTree.states,
      animateStates: false,
    };
    this.handleInputChange = this.handleInputChange.bind(this);
  }

  getAnimatedDots() {
    let states = this.state.states;
    let types = ['Inflow', 'Volume', 'Outflow']
    let dots = []
    for (let stateId in states) {
      let state = states[stateId];
      let dot = []
      dot.push('digraph  {',
      '    node [style="filled"]',
      '    Container [fillcolor="#DAF7A6"]',
      '    Tap [fillcolor="#DAF7A6"]',
      '    Sink [fillcolor="#DAF7A6"]',
      '    inflow [label="Inflow🚰"]',
      '    volume [label="Volume🛁"]',
      '    outflow [label="Outflow🌊"]',
      '    explanation [label="Explanation:\n Something happened" shape=box fillcolor="#FFFFFF"]');

      for (let idx in types)  {
        let type = types[idx];
        if (state.quantities[type].derivative === 'max') {
          dot.push(type.toLowerCase() + 'derivative [label="Derivative:\n➡️max\n+\n0" shape=box fillcolor="#BB8FCE"]');
          
        } else if (state.quantities[type].derivative === '+') {
          dot.push(type.toLowerCase() + 'derivative [label="Derivative:\nmax\n➡️+\n0" shape=box fillcolor="#BB8FCE"]');
        } else if (state.quantities[type].derivative === '0') {
          dot.push(type.toLowerCase() + 'derivative [label="Derivative:\nmax\n+\n➡️0" shape=box fillcolor="#BB8FCE"]');
        }
        if (state.quantities[type].magnitude === 'max') {
          dot.push(type.toLowerCase() + 'magnitude [label="Magnitude:\n➡️max\n+\n0" shape=box fillcolor="#FAAC58"]')
        } else if (state.quantities[type].magnitude === '+') {
          dot.push(type.toLowerCase() + 'magnitude [label="Magnitude:\nmax\n➡️+\n0" shape=box fillcolor="#FAAC58"]')
        } else if (state.quantities[type].magnitude === '0') {
          dot.push(type.toLowerCase() + 'magnitude [label="Magnitude:\nmax\n+\n➡️0" shape=box fillcolor="#FAAC58"]')
        }
      }
      dot.push(
      '    Container -> volume [style=dotted]',
      '    Tap -> inflow [style=dotted]',
      '    Sink -> outflow [style=dotted]',
      '    inflow -> volume [label="I+"]',
      '    volume -> outflow [label="P+"]',
      '    outflow -> volume [label="I-"]',
      '    inflow -> inflowderivative',
      '    volume -> volumederivative',
      '    outflow -> outflowderivative',
      '    inflow -> inflowmagnitude',
      '    volume -> volumemagnitude',
      '    outflow -> outflowmagnitude',
      '    ',
      '}' );
      dots.push(dot);
    }
    return dots;
  }

  getRegularDots() {
    let states = this.state.states;
    let dot = [];
    dot.push(
      'digraph  {',
      'node [style="filled"]');
    for (let stateId in states) {
      let state = states[stateId];
      let inflow = state.quantities['Inflow'].magnitude;
      let dInflow = state.quantities['Inflow'].derivative;
      let volume = state.quantities['Volume'].magnitude;
      let dVolume = state.quantities['Volume'].derivative;
      let outflow = state.quantities['Outflow'].magnitude;
      let dOutflow = state.quantities['Outflow'].derivative;
      dot.push(
        `state${stateId} [label="${stateId}\nInflow: ${inflow}\t\t∂Inflow: ${dInflow}\nVolume: ${volume}\t\t∂Volume: ${dVolume}\nOutflow: ${outflow}\t\t∂Outflow: ${dOutflow}"]`
      );
    }
    // TODO: add connections between states
    dot.push('}');
    return [dot];
  }

  renderDots(animateStates) {
    var dots = animateStates ? this.getAnimatedDots() : this.getRegularDots();
    var dotIndex = 0;
    var graphviz = d3.select("#graph").graphviz()
        .transition(function () {
            return d3.transition("main")
                .ease(d3.easeLinear)
                .delay(100)
                .duration(500);
        })
        .on("initEnd", render);
    
    function render() {
        var dotLines = dots[dotIndex];
        var dot = dotLines.join('');
        graphviz.renderDot(dot)
            .on("end", function () {
                dotIndex = (dotIndex + 1) % dots.length;
                render();
            });
    }
  }

  componentDidUpdate() {
    this.renderDots(!this.state.animateStates);
  }

  componentDidMount() {
    this.renderDots(this.state.animateStates);
  }

  handleInputChange(event) {
    this.setState({
      [event.target.name]: event.target.checked
    });
  }
  

  render() {
    return (
      <div className="App">
        <form>
          <label>Animate states</label>
          <input name="animateStates" type="checkbox" value={this.state.animateStates} onChange={this.handleInputChange}/>
        </form>
        <div id="graph"  style={{textAlign: "center"}} />
      </div>
    );
  }
}

export default App;
