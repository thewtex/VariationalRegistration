/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef __itkVariationalRegistrationMultiResolutionFilter_h
#define __itkVariationalRegistrationMultiResolutionFilter_h

#include "itkImage.h"
#include "itkMultiResolutionPyramidImageFilter.h"
#include "itkVectorResampleImageFilter.h"
#include "itkVariationalRegistrationFilter.h"
#include "itkArray.h"

namespace itk
{
/** \class itk::VariationalRegistrationMultiResolutionFilter
 *  \brief Framework for performing multi-resolution variational registration.
 *
 *  VariationalRegistrationMultiResolutionFilter provides a generic framework
 *  to perform multi-resolution variational registration.
 *
 *  At each resolution level a VariationalRegistrationFilter is used
 *  to register two images by computing the deformation field which will
 *  map a moving image onto a fixed image.
 *
 *  A deformation field is represented as an image whose pixel type is some
 *  vector type with at least N elements, where N is the dimension of
 *  the fixed image. The vector type must support element access via operator
 *  []. It is assumed that the vector elements behave like floating point
 *  scalars.
 *
 *  The internal VariationalRegistrationFilter can be set using
 *  SetRegistrationFilter. By default a DemonsRegistrationFilter is used.
 *
 *  The input fixed and moving images are set via methods SetFixedImage
 *  and SetMovingImage respectively. An initial deformation field maybe set via
 *  SetInitialDisplacementField if is matches the characteristics of the coarsest
 *  pyramid level. If no such assumption can be made (e.g. the deformation field
 *  has the same characteristics as the input images), an initial deformation
 *  field can still be set via SetArbitraryInitialDisplacementField or
 *  SetInput. The filter will then take care of matching the coarsest level
 *  characteristics. If no initial field is set a zero field is used as the
 *  initial condition.
 *
 *  MultiResolutionPyramidImageFilters are used to downsample the fixed
 *  and moving images. A VectorExpandImageFilter is used to upsample
 *  the deformation as we move from a coarse to fine solution.
 *
 *  This class is templated over the fixed image type, the moving image type,
 *  and the Deformation Field type.
 *
 *  \warning This class assumes that the fixed, moving and deformation
 *  field image types all have the same number of dimensions.
 *
 *  \sa MultiResolutionPyramidImageFilter
 *  \sa VectorExpandImageFilter
 *
 *  The current implementation of this class does not support streaming.
 *
 *  \ingroup VariationalRegistration
 */
template< class TFixedImage, class TMovingImage, class TDisplacementField, class  TRealType = float>
class ITK_EXPORT VariationalRegistrationMultiResolutionFilter :
    public ImageToImageFilter< TDisplacementField, TDisplacementField >
{
public:
  /** Standard class typedefs */
  typedef VariationalRegistrationMultiResolutionFilter   Self;
  typedef ImageToImageFilter< TDisplacementField, TDisplacementField >
                                                   Superclass;
  typedef SmartPointer< Self >                     Pointer;
  typedef SmartPointer< const Self >               ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro( VariationalRegistrationMultiResolutionFilter, ImageToImageFilter );

  /** Fixed image type. */
  typedef TFixedImage                               FixedImageType;
  typedef typename FixedImageType::Pointer          FixedImagePointer;
  typedef typename FixedImageType::ConstPointer     FixedImageConstPointer;

  /** Moving image type. */
  typedef TMovingImage                              MovingImageType;
  typedef typename MovingImageType::Pointer         MovingImagePointer;
  typedef typename MovingImageType::ConstPointer    MovingImageConstPointer;

  /** Deformation field image type. */
  typedef TDisplacementField                        DisplacementFieldType;
  typedef typename DisplacementFieldType::Pointer   DisplacementFieldPointer;

  /** ImageDimension. */
  itkStaticConstMacro(ImageDimension, unsigned int, FixedImageType::ImageDimension);

  /** MovingImage image type. */
  typedef unsigned char                               MaskImagePixelType;
  typedef Image< MaskImagePixelType, ImageDimension > MaskImageType;
  typedef typename MaskImageType::Pointer             MaskImagePointer;
  typedef typename MaskImageType::ConstPointer        MaskImageConstPointer;

  /** Internal float image type. */
  typedef Image< TRealType, itkGetStaticConstMacro(ImageDimension) > FloatImageType;

  /** The internal registration type. */
  typedef VariationalRegistrationFilter< FixedImageType, MovingImageType, DisplacementFieldType >
                                                    RegistrationType;
  typedef typename RegistrationType::Pointer        RegistrationPointer;

  /** The default registration type. */
  typedef VariationalRegistrationFilter< FixedImageType, MovingImageType, DisplacementFieldType >
                                                    DefaultRegistrationType;

  /** The fixed multi-resolution image pyramid type. */
  typedef MultiResolutionPyramidImageFilter< FixedImageType, FixedImageType >
                                                    FixedImagePyramidType;
  typedef typename FixedImagePyramidType::Pointer   FixedImagePyramidPointer;

  /** The moving multi-resolution image pyramid type. */
  typedef MultiResolutionPyramidImageFilter< MovingImageType, MovingImageType >
                                                    MovingImagePyramidType;
  typedef typename MovingImagePyramidType::Pointer  MovingImagePyramidPointer;

  /** The mask multi-resolution image pyramid type. */
  typedef MultiResolutionPyramidImageFilter< FloatImageType, FloatImageType >
                                                    MaskImagePyramidType;
  typedef typename MaskImagePyramidType::Pointer    MaskImagePyramidPointer;

  /** The deformation field expander type. */
  typedef VectorResampleImageFilter< DisplacementFieldType, DisplacementFieldType >
                                                    FieldExpanderType;
  typedef typename FieldExpanderType::Pointer       FieldExpanderPointer;

  /** Array containing the number of iterations. */
  typedef Array< unsigned int >                     NumberOfIterationsType;

  /** Set the fixed image. */
  virtual void SetFixedImage( const FixedImageType * ptr );

  /** Get the fixed image. */
  const FixedImageType * GetFixedImage(void) const;

  /** Set the moving image. */
  virtual void SetMovingImage( const MovingImageType * ptr );

  /** Get the moving image. */
  const MovingImageType * GetMovingImage(void) const;

  /** Set the mask image. */
  void SetMaskImage( const MaskImageType * ptr );

  /** Get the mask image. */
  const MaskImageType * GetMaskImage(void) const;

  /** Set initial deformation field to be used as is (no smoothing, no
   *  subsampling at the coarsest level of the pyramid. */
  itkSetObjectMacro( InitialDisplacementField, DisplacementFieldType );

  /** Get the initial deformation field, if set. */
  itkGetConstObjectMacro( InitialDisplacementField, DisplacementFieldType );

  /** Set initial deformation field. No assumption is made on the
   *  input. It will therefore be smoothed and resampled to match the
   *  images characteristics at the coarsest level of the pyramid. */
  virtual void SetArbitraryInitialDisplacementField( DisplacementFieldType * ptr )
    { this->SetInput( ptr ); }

  /** Get output deformation field. */
  // FIXME returns velocities if registration filter is diffeomorphic
  // Should be fixed by: return this->GetDisplacementField();
  const DisplacementFieldType * GetDisplacementField(void)
    { return this->GetOutput(); }

  /** Get the number of valid inputs.  For
   *  VariationalRegistrationMultiResolutionFilter, this checks whether the
   *  fixed and moving images have been set. While
   *  VariationalRegistrationMultiResolutionFilter can take a third input
   *  as an initial deformation field, this input is not a required input. */
  virtual std::vector< SmartPointer<DataObject> >::size_type GetNumberOfValidRequiredInputs() const;

  /** Set the internal registration filter. */
  itkSetObjectMacro( RegistrationFilter, RegistrationType );

  /** Get the internal registration filter. */
  itkGetObjectMacro( RegistrationFilter, RegistrationType );

  /** Set the fixed image pyramid. */
  itkSetObjectMacro( FixedImagePyramid, FixedImagePyramidType );

  /** Get the fixed image pyramid. */
  itkGetObjectMacro( FixedImagePyramid, FixedImagePyramidType );

  /** Set the moving image pyramid. */
  itkSetObjectMacro( MovingImagePyramid, MovingImagePyramidType );

  /** Get the moving image pyramid. */
  itkGetObjectMacro( MovingImagePyramid, MovingImagePyramidType );

  /** Set the mask image pyramid. */
  itkSetObjectMacro( MaskImagePyramid, MaskImagePyramidType );

  /** Get the mask image pyramid. */
  itkGetObjectMacro( MaskImagePyramid, MaskImagePyramidType );

  /** Set number of multi-resolution levels. */
  virtual void SetNumberOfLevels( unsigned int num );

  /** Get number of multi-resolution levels. */
  itkGetConstReferenceMacro( NumberOfLevels, unsigned int );

  /** Get the number of elapsed resolution levels. */
  itkGetConstReferenceMacro( ElapsedLevels, unsigned int );

  /** Set number of iterations per multi-resolution levels. */
  itkSetMacro( NumberOfIterations, NumberOfIterationsType );
  itkSetVectorMacro( NumberOfIterations, unsigned int, m_NumberOfLevels );

  /** Get number of iterations per multi-resolution levels. */
  itkGetConstReferenceMacro( NumberOfIterations, NumberOfIterationsType );

  /** Set the moving image pyramid. */
  itkSetObjectMacro( FieldExpander, FieldExpanderType );

  /** Get the moving image pyramid. */
  itkGetObjectMacro( FieldExpander, FieldExpanderType );

  /** Stop the registration after the current iteration. */
  virtual void StopRegistration();

protected:
  VariationalRegistrationMultiResolutionFilter();
  ~VariationalRegistrationMultiResolutionFilter() {}
  void PrintSelf( std::ostream& os, Indent indent ) const;

  /** Generate output data by performing the registration
   *  at each resolution level. */
  virtual void GenerateData();

  /** The current implementation of this class does not support
   *  streaming. As such it requires the largest possible region
   *  for the moving, fixed and input deformation field. */
  virtual void GenerateInputRequestedRegion();

  /** By default, the output deformation field has the same
   *  spacing, origin and LargestPossibleRegion as the input/initial
   *  deformation field.
   *
   *  If the initial deformation field is not set, the output
   *  information is copied from the fixed image. */
  virtual void GenerateOutputInformation();

  /** The current implementation of this class does not supprot
   *  streaming. As such it produces the output for the largest
   *  possible region. */
  virtual void EnlargeOutputRequestedRegion( DataObject *ptr );

  /** This method returns true to indicate that the registration should
   *  terminate at the current resolution level. */
  virtual bool Halt();

private:
  VariationalRegistrationMultiResolutionFilter(const Self&); //purposely not implemented
  void operator=( const Self& ); //purposely not implemented

  RegistrationPointer        m_RegistrationFilter;
  FixedImagePyramidPointer   m_FixedImagePyramid;
  MovingImagePyramidPointer  m_MovingImagePyramid;
  MaskImagePyramidPointer    m_MaskImagePyramid;
  FieldExpanderPointer       m_FieldExpander;
  DisplacementFieldPointer   m_InitialDisplacementField;

  unsigned int               m_NumberOfLevels;
  unsigned int               m_ElapsedLevels;
  NumberOfIterationsType     m_NumberOfIterations;

  /** Flag to indicate user stop registration request. */
  bool                       m_StopRegistrationFlag;
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkVariationalRegistrationMultiResolutionFilter.hxx"
#endif


#endif
