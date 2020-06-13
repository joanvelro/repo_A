# -*- coding: utf-8 -*-
import dash
import dash_core_components as dcc
import dash_html_components as html
import plotly.graph_objs as go
import pandas as pd
import numpy as np

external_stylesheets = ['https://codepen.io/chriddyp/pen/bWLwgP.css']



N = 100
random_x = np.linspace(0, 1, N)
random_y0 = np.random.randn(N)+5
random_y1 = np.random.randn(N)
random_y2 = np.random.randn(N)-5

random_z0 = np.random.randn(N)+5
random_z1 = np.random.randn(N)
random_z2 = np.random.randn(N)-5


def generate_realtime_monitor(segment_id, TIME_RANGE, df_final ):
    app = dash.Dash(__name__, external_stylesheets=external_stylesheets)
    app.layout = html.Div(children=[
        html.H2(children='Traffic Intensity - SEGMENT_ID ' + str(segment_id)),

        dcc.Graph(
            id='example-graph',
            figure={
                'data': [
                    go.Scatter(
                        x=df_final['FECHA'],
                        y=df_final[str(model_name)],
                        mode='lines+markers',
                        opacity=0.7,
                        name=str(model_name.split('res_')[-1])
                    ) for model_name in df_final.columns.tolist() if model_name.startswith('res_')
                ],

                'layout': go.Layout(
                    xaxis=dict(
                            range = [TIME_RANGE[0],TIME_RANGE[1]]
                    ),
                    yaxis={'title': 'Total Vehicles'},
                    margin={'l': 40, 'b': 40, 't': 10, 'r': 10},
                    legend={'x': 0, 'y': 1},
                    hovermode='closest'
                )
            }
        )
    ])


    return app


