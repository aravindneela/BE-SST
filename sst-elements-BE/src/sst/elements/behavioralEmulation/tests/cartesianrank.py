"""
This Class object sets up a 3-D cartesian grid given the X, Y and Z dimensions.
No real grid is created but it provides methods to find neighbouring ranks and grid coordinates
"""

class CartesianGrid(object):

    def __init__(self, xdim, ydim, zdim):
        
        self.Xdim = xdim
        self.Ydim = ydim
        self.Zdim = zdim

    def neighbourRank(self, myrank, neighbour):

        x = self.myCoordinates(myrank, "x")
        y = self.myCoordinates(myrank, "y")
        z = self.myCoordinates(myrank, "z")       

        if neighbour == "Xplus": x = x+1
        elif neighbour == "Xminus": x = x-1
        elif neighbour == "Yplus": y = y+1
        elif neighbour == "Yminus": y = y-1
        elif neighbour == "Zplus": z = z+1
        elif neighbour == "Zminus": z = z-1
        else: raise Exception("Unknown neighbour "+neighbour)

        if x<0 or x>=self.Xdim or y<0 or y>=self.Ydim or z<0 or z>=self.Zdim:
            return -1;
        else:
            return (x*self.Ydim*self.Zdim + y*self.Zdim + z);

    def myCoordinates(self, myrank, axis):

        if axis == "x" or axis == "X": return myrank/(self.Ydim*self.Zdim)
        elif axis == "y" or axis == "Y": return (myrank%(self.Ydim*self.Zdim))/self.Zdim
        elif axis == "z" or axis == "Z": return myrank%self.Zdim
        else: raise Exception("Unknown axis "+axis)

        return -1

