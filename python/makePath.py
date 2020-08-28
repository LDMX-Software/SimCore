"""Make a path to the data installed with this software"""

def makeLHEPath( ap_mass ) :
    """Get the path to the example data installed with this software given the input
    mass of A' in MeV

    This function will check if the input data directory exists before returning it.

    Parameters
    ----------
    ap_mass : float
        A' mass in MeV

    Returns
    -------
    str :
        Full path to directory containing LHE files to use as vertices

    Warnings
    --------
    The example data does not hold enough vertices to effectively produce large
    scale samples. Only use this for small-scale (<50k events) samples.

    """

    vertex_library_full_path = '${DB_VERTEX_LIB_INSTALL}/mA.'+str(ap_mass/1000)

    import os
    if os.path.isdir(vertex_library_full_path) :
        return vertex_library_full_path
    else :
        import sys
        sys.exit('Requested Vertex Library \'%s\' does not exist!\n%tb'%vertex_library_full_path)
