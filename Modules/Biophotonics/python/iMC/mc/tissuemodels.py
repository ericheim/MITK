'''
Created on Sep 9, 2015

@author: wirkert
'''

import numpy as np

from mc.sim import MciWrapper
from mc.usuag import Ua, UsgJacques


class AbstractTissue(object):
    '''
    Initializes a abstract tissue model"
    '''

    def set_mci_filename(self, mci_filename):
        self._mci_wrapper.set_mci_filename(mci_filename)

    def set_mco_filename(self, mco_filename):
        self._mci_wrapper.set_mco_filename(mco_filename)

    def set_wavelength(self, wavelength):
        self.wavelength = wavelength

    def create_mci_file(self):
        # set layers
        for i, ua in enumerate(self.uas):
            self._mci_wrapper.set_layer(i,  # layer nr
                                        self.ns[i],  # refraction index
                                        self.uas[i](self.wavelength),  # ua
                                        self.usgs[i](self.wavelength)[0],  # us
                                        self.usgs[i](self.wavelength)[1],  # g
                                        self.ds[i])  # d
        # now that the layers have been updated: create file
        self._mci_wrapper.create_mci_file()

    def __str__(self):
        """ Overwrite this method!
        print the current model"""
        model_string = \
                    ""
        return model_string

    def __init__(self, ns, uas, usgs, ds):
        self._mci_wrapper = MciWrapper()

        self.uas = uas
        self.usgs = usgs
        self.ds = ds
        self.ns = ns
        # initially create layers. these will be overwritten as soon
        # as create_mci_file is called.
        for i in enumerate(uas):
            self._mci_wrapper.add_layer()


class GenericTissue(AbstractTissue):
    '''
    Initializes a three layer generic tissue model
    '''

    def set_batch_element(self, batch, element):
        """take the element element of the batch and set the tissue to
        resemble the structure specified by this"""
        self._mci_wrapper.set_nr_photons(batch.nr_photons)
        for i, l in enumerate(batch.layers):
            self.set_layer(i,
                           l[element, 0],  # bvf
                           l[element, 1],  # sao2
                           l[element, 2],  # a_mie
                           l[element, 3],  # a_ray
                           l[element, 4],  # d
                           l[element, 5],  # n
                           l[element, 6])  # g

    def set_layer(self, layer_nr=0,
                  bvf=None, saO2=None, a_mie=None, a_ray=None, d=None,
                  n=None, g=None):
        """Helper function to set one layer."""
        if bvf is None:
            bvf = 0.02
        if saO2 is None:
            saO2 = 0.7
        if a_mie is None:
            a_mie = 10. * 100
        if d is None:
            d = 500. * 10 ** -6
        if a_ray is None:
            a_ray = 0.
        if n is None:
            n = 1.38
        if g is None:
            g = 0.
        # build obejct for absorption coefficient determination
        self.uas[layer_nr].bvf = bvf
        self.uas[layer_nr].saO2 = saO2
        # and one for scattering coefficient
        self.usgs[layer_nr].a_mie = a_mie
        self.usgs[layer_nr].a_ray = a_ray
        self.usgs[layer_nr].g = g
        self.ds[layer_nr] = d
        self.ns[layer_nr] = n

    def __str__(self):
        """print the current model"""
        model_string = ""
        for i, ua in enumerate(self.uas):
            layer_string = "layer " + str(i) + \
                     "    - bvf: " + "%.2f" % self.uas[i].bvf + \
                    "%; SaO2: " + "%.2f" % self.uas[i].saO2 + \
                    "%; a_mie: " + "%.2f" % (self.usgs[i].a_mie / 100.) + \
                    "cm^-1; a_ray: " + "%.2f" % (self.usgs[i].a_ray / 100.) + \
                    "cm^-1; b_mie: " + "%.3f" % self.usgs[i].b_mie + \
                    "; d: " + "%.0f" % (self.ds[i] * 10 ** 6) + "um" + \
                    "; n: " + "%.2f" % (self.ns[i]) + \
                    "; g: " + "%.2f" % (self.usgs[i].g) + "\n"
            model_string += layer_string
        return model_string

    def __init__(self):
        uas = [Ua(), Ua(), Ua()]
        usgs = [UsgJacques(), UsgJacques(), UsgJacques()]
        ds = np.ones_like(uas, dtype=float) * 500.*10 ** -6
        ns = np.ones_like(uas, dtype=float) * 1.38
        super(GenericTissue, self).__init__(ns, uas, usgs, ds)

