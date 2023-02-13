
"""Configuration classes for default photonuclear models"""

from LDMX.SimCore import simcfg

class BertiniModel(simcfg.PhotonuclearModel):
    def __init__(self):
        super().__init__('BertiniModel',
                         'simcore::BertiniModel',
                         'SimCore_PhotonuclearModels')
class BertiniNothingHardModel(simcfg.PhotonuclearModel):
    def __init__(self, nothing_hard_threshold = 200.):
        self.nothing_hard_threshold = nothing_hard_threshold
        super().__init__('BertiniNothingHardModel',
                         'simcore::BertiniNothingHardModel',
                         'SimCore_PhotonuclearModels')
