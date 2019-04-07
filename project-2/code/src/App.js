import React, { Component } from 'react';
import { graphviz} from 'd3-graphviz';
import * as d3 from "d3";
import './App.css';

class App extends Component {  
  componentDidMount() {
    var dotIndex = 0;
    var graphviz = d3.select("#graph").graphviz()
        .transition(function () {
            return d3.transition("main")
                .ease(d3.easeLinear)
                .delay(100)
                .duration(500);
        })
        .logEvents(true)
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

    var dots = [
      [
          'digraph  {',
          '    node [style="filled"]',
          '    Container [fillcolor="#DAF7A6"]',
          '    Tap [fillcolor="#DAF7A6"]',
          '    Sink [fillcolor="#DAF7A6"]',
          '    inflow [label="Inflow🚰"]',
          '    volume [label="Volume🛁"]',
          '    outflow [label="Outflow🌊"]',
          '    inflowgradient [label="Gradient:\n➡️+\n0" shape=box fillcolor="#BB8FCE"]',
          '    volumegradient [label="Gradient:\nmax\n+\n0" shape=box fillcolor="#BB8FCE"]',
          '    outflowgradient [label="Gradient:\nmax\n+\n0" shape=box fillcolor="#BB8FCE"]',
          '    Container -> volume [style=dotted]',
          '    Tap -> inflow [style=dotted]',
          '    Sink -> outflow [style=dotted]',
          '    inflow -> volume [label="I+"]',
          '    volume -> outflow [label="P+"]',
          '    outflow -> volume [label="I-"]',
          '    inflow -> inflowgradient',
          '    volume -> volumegradient',
          '    outflow -> outflowgradient',
          '    ',
          '}'
      ]
    ]
  }

  render() {
    return (
      <div className="App">
        <div id="graph"  style={{textAlign: "center"}} />
      </div>
    );
  }
}

export default App;
