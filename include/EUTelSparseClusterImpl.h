// -*- mode: c++; mode: auto-fill; mode: flyspell-prog; -*-
/*
 *   This source code is part of the Eutelescope package of Marlin.
 *   You are free to use this source files for your own development as
 *   long as it stays in a public research context. You are not
 *   allowed to use it for commercial purpose. You must put this
 *   header with author names in all development based on this file.
 *
 */

#ifndef EUTELSPARSECLUSTERIMPL_H
#define EUTELSPARSECLUSTERIMPL_H

// personal includes ".h"
#include "EUTELESCOPE.h"
#include "EUTelVirtualCluster.h"
#include "EUTelBaseSparsePixel.h"
#include "EUTelSimpleSparsePixel.h"
#include "EUTelSparseDataImpl.h"
#include "EUTelExceptions.h"

// marling includes ".h"
#include <marlin/Exceptions.h>

// lcio includes <.h>
#include <IMPL/TrackerDataImpl.h>

// system includes <>
#include <iostream>
#include <limits>
#include <memory>
#include <vector>

namespace eutelescope {

  //! Implementation of a cluster made of sparsified pixels
  /*! This is a group of sparsified pixels having fulfilling the
   *  proximity requirement.
   *
   *  The shape and the size of the cluster is a priori undefined and
   *  the seed pixel is the one with the highest signal in the
   *  cluster.
   *
   *  The sparse pixel information are stored in the TrackerData using
   *  one of the sparse pixel implementation, via the addPixel()
   *  method. 
   *
   *  @Author Antonio Bulgheroni, INFN <mailto:antonio.bulgheroni@gmail.com>
   *  @Version $Id: EUTelSparseClusterImpl.h,v 1.3 2007-08-21 13:18:05 bulgheroni Exp $
   */
  template<class PixelType>
  class EUTelSparseClusterImpl : public EUTelVirtualCluster {
  public:
    //! Default constructor
    EUTelSparseClusterImpl(IMPL::TrackerDataImpl * data);

    //! Destructor
    virtual ~EUTelSparseClusterImpl() { /* NOOP */ ; }

    //! Set the pixel noise values
    /*! This method is used to set the noise values. The Fixed Frame
     *  cluster implementation does not have in the TrackerData the
     *  noise value of the pixels. This method allows to attach at the
     *  current cluster a vector of floats with one entry for each
     *  pixel representing the noise value. The noise values in the
     *  vector must be ordered with the same order the pixel signals
     *  have been loaded into the TrackerData.
     *  
     *  Depending on the type of sparsified pixel, the noise values
     *  can already contained in there. In this case, this method is
     *  returning immediately.
     *
     *  When this method is called, the _noiseSetSwitch is set to true
     *  and all the other methods calling noise features will be made
     *  available. 
     *
     *  @param noiseValues The vector containing the pixel noise.
     */ 
    void setNoiseValues(std::vector<float > noiseValues);
    
    //! Get the detector ID
    /*! This method is used to get from the CellID the detector
     *  identification number
     *
     *  @return the detector ID 
     */
    inline int getDetectorID()  const {
      int   rhs = 0;
      lcio::long64 mask  = 0x1F;
      lcio::long64 cell0 = static_cast<lcio::long64> (_trackerData->getCellID0());
      return static_cast<int> ( ( cell0 & mask ) >> rhs );
      
    }

    //! Get the cluster identification number
    /*! This method is used to return the cluster identification number
     *  @return the clusterID
     */
    inline int getClusterID() const {
      int rhs = 5;
      lcio::long64 mask = 0x1FE0;
      lcio::long64 cell0 = static_cast<lcio::long64> (_trackerData->getCellID0());
      return static_cast<int> ( ( cell0  & mask ) >> rhs );
    }
    
    //! Get the seed pixel coordinate in the local FoR
    /*! Regardless the kind of cluster in use, it is always possible
     *  to define the seed pixel as the one with the highest signal. 
     *
     *  @param xSeed The coordinate along x of the seed pixel in the
     *  local frame of reference
     *  @param ySeed The coordinate along y of the seed pixel in the
     *  local frame of reference
     */ 
    void getSeedCoord(int& xSeed, int& ySeed) const ;
    
    //! Get the cluster dimensions
    /*! For each cluster type is always possible to define the
     *  external sizes. 
     *
     *  @param xSize The size along x
     *  @param ySize The size along y
     */ 
    virtual void getClusterSize(int& xSize, int& ySize) const { ; } 


    //! Return the cluster quality
    /*! It returns the cluster quality using the ClusterQuality enum.
     *  
     *  @return The cluster quality
     *
     *  @see ClusterQuality
     */
    ClusterQuality getClusterQuality() const {
      int rhs = 18;
      lcio::long64 mask   = 0x7C0000;
      lcio::long64 cell0  = static_cast<lcio::long64> (_trackerData->getCellID0());
      return static_cast<ClusterQuality> ( ( cell0 & mask ) >> rhs );
    }

    //! Return the distance from another 
    /*! This method return the distance between this and another
     *  cluster 
     *
     *  @param  clu The other cluster
     *  @return The distance between @a this and @a clu
     */ 
    float getDistance(EUTelVirtualCluster* clu) const  { return 0; }

    //! Return the radius of the external circle
    /*!
     *  @return The radius of the external circle
     */ 
    virtual float getExternalRadius() const { return  0; }

    //! Return the total charge
    /*!
     *  @return The total integrated charge
     */
    float getTotalCharge() const ;

    //! Return the seed pixel charge
    /*! 
     *  @return the charge of the seed pixel
     */ 
    virtual float getSeedCharge() const ;

    //! Return the charge of cluster with N pixels
    /*! This method can be used to return the charge integrated into
     *  the cluster when only considering the N most significant
     *  pixels.
     *
     *  @return the charge of cluster with N pixels
     *  @param nPixel The number of pixels to consider
     */ 
    virtual float getClusterCharge(int nPixel) const  { return 0.; }

    //! Calculate the cluster charge with different number of pixels
    /*! This method is a better and faster replacement of the
     *  getClusterCharge(int) method. This one is actually avoiding to
     *  re-sort the signal vector all the times it is called. 
     *
     *  @param nPixels The list of number of pixels
     *  @return The charges for each number of pixels
     */ 
    virtual std::vector<float > getClusterCharge(std::vector<int > nPixels) const { std::vector<float > vec; return vec; }

    //! Return the charge of a subset of the cluster
    /*! Whatever shape the cluster has it is always possible to define
     *  a rectangular frame centered on the seed pixel. With this
     *  method the charge of this frame is calculated.
     *
     *  @param  xSize Odd number to define the x size of the subframe
     *  @param  ySize Odd number to define the y size of the subframe
     *  @return The charge of the cluster subframe
     */ 
    virtual float getClusterCharge(int xSize, int ySize) const  { return 0.; }

    //! Return the center of gravity shift from the seed coordinates
    /*! Having a charge distribution it is possible to calculate the
     *  charge center of gravity of the cluster. This will not
     *  correspond to the center of the cluster. This method returns
     *  the shift in both directions one has to apply to the cluster
     *  center to find the center of gravity
     *
     *  @param x Shift along x
     *  @param y Shift along y
     */ 
    virtual void  getCenterOfGravityShift(float& x, float& y) const  { ; }

    //! Return the CoG shift using only a subregion of the cluster
    /*! As the method above, but this uses only a subset of pixel in
     *  the cluster: the one belonging to a frame @a xSize width @a
     *  ySize long around the center
     *
     *  @param x Shift along x
     *  @param y Shift along y
     *  @param xSize Frame size along x
     *  @param ySize Frame size along y
     */ 
    virtual void  getCenterOfGravityShift(float& x, float& y, 
					  int xSize, int ySize) const  { ; }

    //! Return the CoG shift using only the nth higher pixels
    /*! As the above method, but this uses only the @n pixels with the
     *  highest signal for CoG calculation
     *
     *  @param x Shift along x
     *  @param y Shift along y
     *  @param n Number of pixels to be used
     */
    virtual void  getCenterOfGravityShift(float& x, float& y,
					  int n) const   { ; }

    //! Return the CoG coordinates
    /*! This method adds already to the shift the coordinates of the
     *  cluster center
     *
     *  @param x CoG coordinate along x
     *  @param y CoG coordinate along y
     */ 
    virtual void  getCenterOfGravity(float& x, float& y) const  { ; }

    //! Set the cluster quality
    /*! Used to set the cluster quality using the ClusterQuality enum.
     *  
     *  @see eutelescope::ClusterQuality
     */
    virtual void setClusterQuality(ClusterQuality)  { ;}

    //! Get the pixel noise values
    /*! This method returns the pixel noise value vector if it was
     *  properly set. Otherwise it throws a DataNotAvailableException.
     *
     *  @return a vector of float with the pixel noise values.
     */
    std::vector<float > getNoiseValues() const;   

   //! Get the cluster noise
    /*! This method is used to calculate the cluster noise.
     *  See the implementation for the way the cluster noise is
     *  calculated. 
     *
     *  @return A float value representing the cluster noise.
     */
    virtual float getClusterNoise() const    { return 0.; }

    //! Get the cluster SNR
    /*! This method is used to calculate the cluster signal to noise
     *  ratio. 
     *
     *  @return The cluster SNR for the current cluster
     */ 
    virtual float getClusterSNR() const   { return 0.; }

    //! Get seed pixel SNR
    /*! This method is used to calculate the seed pixel signal to
     *  noise ratio. Note that depending on the cluster
     *  implementation, the definition of seed may differ.
     *
     *  @return The seed pixel SNR
     */
    virtual float getSeedSNR() const { return  0. ; }

    //! Get the cluster N SNR
    /*! This method returns the SNR of the cluster considering only
     *  the N most significant pixels. The pixel significance is based
     *  on a signal (and not SNR) basis.
     *
     *  @param nPixel The number of pixel to consider in the cluster
     *  @return The cluster N SNR.
     */ 
    virtual float getClusterSNR(int nPixel) const  {return 0.; }

    //! Get the cluster N x M SNR
    /*! This method returns the SNR when considering only a
     *  rectangular subframe of N x M pixel centered around the seed
     *  pixel. 
     *
     *  @param xSize Odd number to define the x size of the subframe
     *  @param ySize Odd number to define the y size of the subframe
     *  @return The SNR of the cluster subframe
     */
    virtual float getClusterSNR(int xSize, int ySize) const { return 0. ; }

    //! Calculate the cluster SNR with different number of pixels
    /*! This method is a better and faster replacement of the
     *  getClusterSNR(int) method. This one is actually avoiding to
     *  re-sort the signal vector all the times it is called. 
     *
     *  @param nPixels The list of number of pixels
     *  @return The SNRs for each number of pixels
     */ 
    virtual std::vector<float > getClusterSNR(std::vector<int > nPixels) const   { std::vector<float > vec; return vec;} 
    
    //! Return a pointer to the TrackerDataImpl
    /*! This method is used to expose to the public the
     *  TrackerDataImpl member.
     *
     *  @return The pointer of _trackerData
     */
    virtual IMPL::TrackerDataImpl * trackerData()  { return _trackerData; } 
    
    //! Print
    /*! This method is used to print out the content of the clusters
     * 
     *  @param os The input output stream
     */
    virtual void print(std::ostream& os) const { ; }

    ////////////////////////////////////////////////////////
    //                                                    //
    //                IMPLEMENTATION                      //
    //                  SPECIFIC                          //
    //                                                    //
    ////////////////////////////////////////////////////////

    //! Get the number of sparse pixels in the collection
    /*! This utility can be used to know how many pixels are contained
     *  in the TrackerData.
     *
     *  @return the size of TrackerData measured in sparse
     *  pixels. 
     */ 
    unsigned int size() const ;
    
    //! Get the type of sparsified pixel
    /*! This method is used to get the type of sparsified pixel
     *  contained in the cluster. 
     *
     *  It is implementation specific and it is not available in the
     *  virtual interface since not all kind of clusters are made by
     *  sparsified pixels
     *
     *  @return The sparse pixel type using the SparsePixelType enum
     *
     *  @see SparsePixelType
     */
    SparsePixelType getSparsePixelType() const {
      int rhs = 13;
      lcio::long64 mask   = 0x3E000;
      lcio::long64 cell0  = static_cast<lcio::long64> (_trackerData->getCellID0());
      return static_cast<SparsePixelType> ( ( cell0 & mask ) >> rhs );
    }

    //! Get one of the sparse pixel
    /*! This method is used to get one of the sparse pixel contained
     *  into the TrackerData. 
     *
     *  @param index Index of the sparse pixel within the collection
     *
     *  @return A EUTelSparsePixel with the information concerning the
     *  pixel number @a index in the collection
     */ 
    PixelType * getSparsePixelAt(unsigned int index, PixelType * pixel);

  protected:

    //! The number of elements in the data structure
    unsigned int _nElement;

    //! The sparse pixel type enumerator
    SparsePixelType _type;

  private:
    
    //! Noise values vector
    std::vector<float > _noiseValues;

    //! Noise set switch
    /*! By default this switch is set to false and it is disabling all
     *  methods referring to the _noiseValue. This bool is set to true
     *  only when the setNoiseValues() is called and all the
     *  crosschecks have been passed.
     */
    bool _noiseSetSwitch;

  };

  template<>
  EUTelSimpleSparsePixel * 
  EUTelSparseClusterImpl<EUTelSimpleSparsePixel>::getSparsePixelAt(unsigned int index, EUTelSimpleSparsePixel * pixel); 


  template<class PixelType>
  EUTelSparseClusterImpl<PixelType>::EUTelSparseClusterImpl(IMPL::TrackerDataImpl * data) {
    // before proceeding we need to verify if the template parameter
    // corresponds to a valid sparsified pixel. To be valid, this has
    // to inherit from EUTelBaseSparsePixel
    EUTelBaseSparsePixel * goodPixelType;
    std::auto_ptr<PixelType>    currentPixelType(new PixelType);
    if ( goodPixelType = dynamic_cast<PixelType *> ( currentPixelType.get() ) ) {
      _nElement       = goodPixelType->getNoOfElements();
      _type           = goodPixelType->getSparsePixelType();
      _trackerData    = data;
      _noiseSetSwitch = false;
      _noiseValues.clear();
    } else {
      throw InvalidParameterException(std::string("The template parameter is not valid"));
    }
  }

  template<class PixelType>
  unsigned int EUTelSparseClusterImpl<PixelType>::size()  const {
    return _trackerData->chargeValues().size() / _nElement;
  }

  template<class PixelType>
  void EUTelSparseClusterImpl<PixelType>::setNoiseValues(std::vector<float > noiseValues) {
    if ( noiseValues.size() != size() ) {
      _noiseSetSwitch = false;
      throw IncompatibleDataSetException("The noiseValues size is different from the number of pixel in the cluster");
    }
    
    _noiseValues    = noiseValues;
    _noiseSetSwitch = true;

  }

  template<class PixelType>
  std::vector<float > EUTelSparseClusterImpl<PixelType>::getNoiseValues() const {
    if ( ! _noiseSetSwitch ) throw DataNotAvailableException("No noise values set");
    return _noiseValues;
  }

  template<class PixelType>
  void EUTelSparseClusterImpl<PixelType>::getSeedCoord(int& xSeed, int& ySeed) const {
    unsigned int   maxIndex  = 0;
    float          maxSignal = -1 * std::numeric_limits<float>::max();
    for ( unsigned int index = 0; index < size() ; index++ ) {
      EUTelBaseSparsePixel * pixel = static_cast<EUTelBaseSparsePixel*> (&getSparsePixelAt(index));
      if ( pixel->getSignal() > maxSignal ) {
 	maxSignal = pixel->getSignal();
 	maxIndex  = index;
      }
    }
    EUTelBaseSparsePixel * seedPixel = static_cast<EUTelBaseSparsePixel*> (&getSparsePixelAt(maxIndex));
    xSeed = seedPixel->getXCoord();
    ySeed = seedPixel->getYCoord();
  }

  template<class PixelType>
  float EUTelSparseClusterImpl<PixelType>::getTotalCharge() const {
    float charge = 0;
    for ( unsigned int index = 0; index < size() ; index++ ) {    
      charge += (static_cast<EUTelBaseSparsePixel*> (&getSparsePixelAt(index)))->getSignal();
    }
    return charge;
  }

  template<class PixelType>
  float EUTelSparseClusterImpl<PixelType>::getSeedCharge() const {
    unsigned int   maxIndex  = 0;
    float          maxSignal = -1 * std::numeric_limits<float>::max();
    for ( unsigned int index = 0; index < size() ; index++ ) {
      EUTelBaseSparsePixel * pixel = static_cast<EUTelBaseSparsePixel*> (&getSparsePixelAt(index));
      if ( pixel->getSignal() > maxSignal ) {
	maxSignal = pixel->getSignal();
	maxIndex  = index;
      }
    }
    return (static_cast<EUTelBaseSparsePixel*> (&getSparsePixelAt(maxIndex)))->getSignal();
  }


  

  ////////////////////////////////////////////////////////
  //                                                    //
  //                     TEMPLATE                       //
  //                  SPECIALIZATION                    //
  //                                                    //
  ////////////////////////////////////////////////////////
  
  template<>
  inline EUTelSimpleSparsePixel * EUTelSparseClusterImpl<EUTelSimpleSparsePixel>::getSparsePixelAt(unsigned int index,
												   EUTelSimpleSparsePixel * pixel)  {
    pixel->setXCoord( static_cast<int> ( _trackerData->chargeValues()[index * _nElement]     ) );
    pixel->setYCoord( static_cast<int> ( _trackerData->chargeValues()[index * _nElement + 1] ) );
    pixel->setSignal( static_cast<int> ( _trackerData->chargeValues()[index * _nElement + 2] ) );
    return pixel;
  }
}

#endif
