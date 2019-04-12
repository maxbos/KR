import React, { Component } from 'react';
import { graphviz} from 'd3-graphviz';
import * as d3 from "d3";
import { World } from './qr';
import './App.css';

class App extends Component {  
  constructor(props) {
    super(props);
    this.state = {
      states: this.convertMagnitudesAndDerivatives(new World().stateTree.states),
      animateStates: false,
    };
    console.log(this.state.states);
    this.handleInputChange = this.handleInputChange.bind(this);
  }

  deriveExplanation(state) {
    let explanation = ''
    let im = state.quantities['Inflow'].magnitude;
    let id = state.quantities[['Inflow']].derivative;
    let vm = state.quantities['Volume'].magnitude;
    let vd = state.quantities['Volume'].derivative;
    let om = state.quantities['Outflow'].magnitude;
    let od = state.quantities['Outflow'].magnitude;
    if (vm === '0') {
      explanation += "The volume in the bathtub is empty\n";
      if (im === '0' && id === '0') {
        explanation += "because no water is flowing in.\n";
      } else if (om !== '0' || od !== '0') {
        explanation += "because water outflow was higher than inflow";
      } else {
        explanation += "but water starts flowing in";
      }
    } else if (vd === '+') {
      explanation += "The volume in the bathtub is rising\n";
      if (im !== '0' && om === '0') {
        explanation += "because water is flowing in and no water is flowing out";
      } else {
        explanation += "because water is flowing in faster than flowing out";
      }
    } else if (vd === '0') {
      explanation += "The volume in the bathtub is steady\n";
      explanation += "because water is flowing in and out in the same pace";
    } else if (vd === '-') {
      explanation += "The volume in the bathtub is declining\n";
      explanation += "because water is flowing out faster than flowing in";
    }
    if (vm === 'max') {
      explanation += "\nAnd outflow has reached it's maximum\n";
      explanation += "because volume has reached it's maximum";
    }
    return explanation;
  }

  getAnimatedDots() {
    let states = this.state.states;
    let types = ['Inflow', 'Volume', 'Outflow']
    let dots = []
    for (let stateId in states) {
      let state = states[stateId];
      let log = this.deriveExplanation(state);
      let dot = []
      dot.push('digraph  {',
      '    node [style="filled"]',
      '    Container [fillcolor="#DAF7A6"]',
      '    Tap [fillcolor="#DAF7A6"]',
      '    Sink [fillcolor="#DAF7A6"]',
      '    inflow [label="Inflow🚰"]',
      '    volume [label="Volume🛁"]',
      '    outflow [label="Outflow🌊"]',
      `    explanation [label="State: ${stateId}\nExplanation:\n${log}" shape=box fillcolor="#FFFFFF"]`);

      for (let idx in types)  {
        let type = types[idx];
        if (state.quantities[type].derivative === '+') {
          dot.push(type.toLowerCase() + 'derivative [label="Derivative:\n➡️+\n0\n-" shape=box fillcolor="#BB8FCE"]');
        } else if (state.quantities[type].derivative === '0') {
          dot.push(`${type.toLowerCase()}derivative [label="Derivative:\n+\n➡️0\n-" shape=box fillcolor="#BB8FCE"]`)
        } else if (state.quantities[type].derivative === '-') {
          dot.push(`${type.toLowerCase()}derivative [label="Derivative:\n+\n0\n➡️-" shape=box fillcolor="#BB8FCE"]`)
        }
        let maxValue = (type === 'Volume' || type === 'Outflow') ? 'max\n' : '';
        if (state.quantities[type].magnitude === 'max') {
          dot.push(type.toLowerCase() + 'magnitude [label="Magnitude:\n➡️max\n+\n0" shape=box fillcolor="#FAAC58"]')
        } else if (state.quantities[type].magnitude === '+') {
          dot.push(type.toLowerCase() + `magnitude [label="Magnitude:\n${maxValue}➡️+\n0" shape=box fillcolor="#FAAC58"]`)
        } else if (state.quantities[type].magnitude === '0') {
          dot.push(type.toLowerCase() + `magnitude [label="Magnitude:\n${maxValue}+\n➡️0" shape=box fillcolor="#FAAC58"]`)
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
    let connections = [];
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
        `state${stateId} [label="${stateId}\nInflow: ${inflow}\t\t∂Inflow: ${dInflow}\nVolume: ${volume}\t\t∂Volume: ${dVolume}\nOutflow: ${outflow}\t\t∂Outflow: ${dOutflow}"]\n`
      );
      for (let idx in state.children) {
        let [childId, log] = state.children[idx];
        connections.push(`state${stateId} -> state${childId} [label=<
          <table width="200" border="0" cellborder="1" cellspacing="0">
            <tr><td bgcolor="#FAAC58"><font POINT-SIZE="8">${log.join('<br />')}</font></td></tr>
          </table>>]\n`);
      }
    }
    dot.push(...connections);
    dot.push('}');
    console.log(JSON.stringify(dot.join('\n')));
    return [dot];
  }

  convertMagnitudesAndDerivatives(states) {
    for (let stateId in states) {
      let state = states[stateId];
      let types = ['Inflow', 'Volume', 'Outflow']
      for (let idx in types)  {
        let type = types[idx];
        let mag = state.quantities[type].magnitude;
        let der = state.quantities[type].derivative;
        state.quantities[type].magnitude = mag === 1 ? '+' : mag === 2 ? 'max' : mag.toString();
        state.quantities[type].derivative = der === -1 ? '-' : der === 0 ? '0' : '+';
      }
    }
    return states
  }

  renderDots(animateStates) {
    const delay = {
      animateStates,
      counter: 0,
      f() {
        const animateddelay = (this.counter > 3) ? 5000 : 0;
        this.counter++;
        return this.animateStates ? animateddelay : 0;
      },
    };
    const unbounded = delay.f;
    const bounded = unbounded.bind(delay);

    var dots = animateStates ? this.getAnimatedDots() : this.getRegularDots();
    var dotIndex = 0;
    const div = d3.select("#graph");
    var graphviz = div.graphviz({
      totalMemory: 1073741824,
      fit: true,
      scale: animateStates ? 1 : 0.5,
      tweenShapes: false,
      tweenPaths: false,
      convertEqualSidedPolygons: false,
      tweenPrecision: 10,
    })
        .transition(function () {
          return d3.transition("main")
              .ease(d3.easeLinear)
              .delay(bounded)
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

  shouldComponentUpdate(nextProps, nextState) {
    this.renderDots(nextState.animateStates);
    return true;
  }

  componentDidMount() {
    this.renderDots(this.state.animateStates);
  }

  componentDidUpdate() {
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
          {this.state.animateStates ? '' : 
            <p>Note that the graph is quite large and therefore slow, you can access it <a href="graph.png">here</a> as PNG.</p>
          }
        </form>
        <div id="graph" style={{textAlign: "center"}} />
      </div>
    );
  }
}

export default App;