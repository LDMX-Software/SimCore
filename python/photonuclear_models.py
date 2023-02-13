
"""Configuration classes for sensitive detectors"""

from LDMX.SimCore import simcfg

class BertiniModel(simcfg.PhotonuclearModel):
    def __init__(self):
        super().__init__('BertiniModel',
                         'simcore::BertiniModel',
                         'SimCore_PhotonuclearModels')
