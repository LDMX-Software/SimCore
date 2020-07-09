"""Internal configuration module for simulation objects

The simulation requires a lot more detailed configuration than
the other processors, so we have two extra objects that require
their own python classes.
"""

class UserAction: 
    """Object that stores parameters for a UserAction

    Parameters
    ----------
    instance_name : str
        Unique name for this particular instance of a UserAction
    class_name : str
        Name of C++ class that this UserAction should be
    """

    def __init__(self, instance_name, class_name):
        self.class_name    = class_name
        self.instance_name = instance_name

    def __str__(self): 
        """Stringify this UserAction

        Returns
        -------
        str
            A human-readable version of this UserAction printing all its attributes
        """

        string = "UserAction (" + self.__repr__() + ")\n"
        string += " Parameters: \n"
        for k, v in self.__dict__.items(): 
            string += "  %s : %s \n" % (k, v)

        return string

    def __repr__(self):
        """A shorter string representation of this UserAction

        Returns
        -------
        str
            Just printing its instance and class names
        """

        return '%s of class %s' % (self.instance_name, self.class_name)

class PrimaryGenerator: 
    """Object that stores parameters for a PrimaryGenerator

    Parameters
    ----------
    instance_name : str
        Unique name for this particular instance of a PrimaryGenerator
    class_name : str
        Name of C++ class that this PrimaryGenerator should be
    """

    def __init__(self, instance_name, class_name):
        self.class_name    = class_name
        self.instance_name = instance_name

    def __str__(self): 
        """Stringify this PrimaryGenerator

        Returns
        -------
        str
            A human-readable version of this PrimaryGenerator printing all its attributes
        """


        string = "PrimaryGenerator (" + self.__repr__() + ")\n" 
        string += " Parameters: \n"
        for k, v in self.__dict__.items(): 
            string += "  %s : %s \n" % (k, v)

        return string

    def __repr__(self):
        """A shorter string representation of this PrimaryGenerator

        Returns
        -------
        str
            Just printing its instance and class names
        """

        return '%s of class %s' % (self.instance_name, self.class_name)

class DarkBrem:
    """Storage for parameters of dark brem process

    Attributes
    ----------
    APrimeMass : float
        Mass of A' in MeV
    library_path : str
        Path to directory of LHE files containing dark brem vertices
    enable : bool
        Should we use the custom Geant4 dark brem process?
    method : int
        Interpretation method for LHE files
    threshold : float
        Minimum energy that electron should have for dark brem to have nonzero xsec
    epsilon : float
        Epsilon for dark brem xsec calculation
    """

    def __init__(self) : 

        self.APrimeMass   = 0.
        self.library_path = ''
        self.enable       = False #off by default
        self.method       = 1
        self.threshold    = 0.
        self.epsilon      = 1.
        self.only_one_per_event = False

    def activate(self, apMass, lib_path ) :
        """Activate the dark brem process with the input A' mass [MeV] and path to vertex library"""

        self.enable       = True
        self.APrimeMass   = apMass
        self.library_path = lib_path
