/*
 Copyright (c) 2010-2015, Paul Houx - All rights reserved.
 This code is intended for use with the Cinder C++ library: http://libcinder.org

 This file is part of Cinder-Warping.

 Cinder-Warping is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Cinder-Warping is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Cinder-Warping.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "cinder/Area.h"
#include "cinder/Color.h"
#include "cinder/DataSource.h"
#include "cinder/DataTarget.h"
#include "cinder/Matrix.h"
#include "cinder/Rect.h"
#include "cinder/Vector.h"

#include "cinder/gl/gl.h"

#include <atomic>
#include <vector>

 // forward declarations

namespace cinder {
class XmlTree;

namespace app {
class	KeyEvent;
class	MouseEvent;
}

namespace gl {
class	Texture2d;
typedef	std::shared_ptr<Texture2d>	Texture2dRef;
}
}

//

namespace ph {
namespace warping {

typedef std::shared_ptr<class Warp>			WarpRef;
typedef std::vector<WarpRef>				WarpList;
typedef WarpList::iterator					WarpIter;
typedef WarpList::const_iterator			WarpConstIter;
typedef WarpList::reverse_iterator			WarpReverseIter;
typedef WarpList::const_reverse_iterator	WarpConstReverseIter;

class Warp
	: public std::enable_shared_from_this < Warp > {
public:
	typedef enum { UNKNOWN, BILINEAR, PERSPECTIVE, PERSPECTIVE_BILINEAR } WarpType;
public:
	Warp( WarpType type = UNKNOWN );
	virtual ~Warp( void );

	//!
	static bool			isEditModeEnabled() { return (bool)sIsEditMode; };
	static void			enableEditMode( bool enabled = true ) { sIsEditMode = enabled; };
	static void			disableEditMode() { sIsEditMode = false; };
	static void			toggleEditMode() { sIsEditMode = !sIsEditMode; };

	//! returns the type of the warp
	WarpType			getType() const { return mType; };

	//! returns a shared pointer to this warp
	WarpRef				getPtr() { return shared_from_this(); }

	//!
	virtual ci::XmlTree	toXml() const;
	//!
	virtual void		fromXml( const ci::XmlTree &xml );

	//! get the width of the content in pixels
	int					getWidth() const { return mWidth; };
	//! get the height of the content in pixels
	int					getHeight() const { return mHeight; };
	//! get the width and height of the content in pixels
	ci::ivec2			getSize() const { return ci::ivec2( mWidth, mHeight ); };
	//! get the width and height of the content in pixels
	ci::Area			getBounds() const { return ci::Area( 0, 0, mWidth, mHeight ); };

	//! set the width of the content in pixels
	virtual void		setWidth( int w ) { setSize( w, mHeight ); }
	//! set the height of the content in pixels
	virtual void		setHeight( int h ) { setSize( mWidth, h ); }
	//! set the width and height of the content in pixels
	virtual void		setSize( const ci::ivec2 &size ) { setSize( size.x, size.y ); }
	//! set the width and height of the content in pixels
	virtual void		setSize( int w, int h );

	//! reset control points to undistorted image
	virtual void		reset() = 0;
	//! setup the warp before drawing its contents
	virtual void		begin() = 0;
	//! restore the warp after drawing
	virtual void		end() = 0;

	//! draws a warped texture
	void				draw( const ci::gl::Texture2dRef &texture );
	//! draws a specific area of a warped texture
	void				draw( const ci::gl::Texture2dRef &texture, const ci::Area &srcArea );
	//! draws a specific area of a warped texture to a specific region
	virtual void		draw( const ci::gl::Texture2dRef &texture, const ci::Area &srcArea, const ci::Rectf &destRect ) = 0;

	//! adjusts both the source area and destination rectangle so that they are clipped against the warp's content
	bool				clip( ci::Area &srcArea, ci::Rectf &destRect ) const;

	//! returns the coordinates of the specified control point
	virtual ci::vec2	getControlPoint( unsigned index ) const;
	//! sets the coordinates of the specified control point
	virtual void		setControlPoint( unsigned index, const ci::vec2 &pos );
	//! moves the specified control point 
	virtual void		moveControlPoint( unsigned index, const ci::vec2 &shift );
	//! select one of the control points
	virtual void		selectControlPoint( unsigned index );
	//! deselect the selected control point
	virtual void		deselectControlPoint();
	//! returns the index of the closest control point, as well as the distance in pixels
	virtual unsigned	findControlPoint( const ci::vec2 &pos, float *distance ) const;

	//! set the width and height in pixels of the content of all warps
	static void			setSize( const WarpList &warps, int w, int h );
	//! set the width and height in pixels of the content of all warps
	static void			setSize( const WarpList &warps, const ci::ivec2 &size ) { setSize( warps, size.x, size.y ); }

	//! checks all warps and selects the closest control point
	static void			selectClosestControlPoint( const WarpList &warps, const ci::ivec2 &position );

	//! draw a control point in the correct preset color
	static void			drawControlPoint( const ci::vec2 &pt, bool selected = false, bool attached = false );
	//! draw a control point in the specified color
	static void			drawControlPoint( const ci::vec2 &pt, const ci::Color &clr, float scale = 1.0f );

	//! read a settings xml file and pass back a vector of Warps
	static WarpList		readSettings( const ci::DataSourceRef &source );
	//! write a settings xml file
	static void			writeSettings( const WarpList &warps, const ci::DataTargetRef &target );

	//! handles mouseMove events for multiple warps
	static bool			handleMouseMove( WarpList &warps, ci::app::MouseEvent &event );
	//! handles mouseDown events for multiple warps
	static bool			handleMouseDown( WarpList &warps, ci::app::MouseEvent &event );
	//! handles mouseDrag events for multiple warps
	static bool			handleMouseDrag( WarpList &warps, ci::app::MouseEvent &event );
	//! handles mouseUp events for multiple warps
	static bool			handleMouseUp( WarpList &warps, ci::app::MouseEvent &event );

	//! handles keyDown events for multiple warps
	static bool			handleKeyDown( WarpList &warps, ci::app::KeyEvent &event );
	//! handles keyUp events for multiple warps
	static bool			handleKeyUp( WarpList &warps, ci::app::KeyEvent &event );

	//! handles resize events for multiple warps
	static bool			handleResize( WarpList &warps );

	virtual void		mouseMove( ci::app::MouseEvent &event );
	virtual void		mouseDown( ci::app::MouseEvent &event );
	virtual void		mouseDrag( ci::app::MouseEvent &event );
	virtual void		mouseUp( ci::app::MouseEvent &event );

	virtual void		keyDown( ci::app::KeyEvent &event );
	virtual void		keyUp( ci::app::KeyEvent &event );

	virtual void		resize();
protected:
	//! draw the warp and its editing interface
	virtual void		draw( bool controls = true ) = 0;

protected:
	WarpType		mType;

	bool			mIsDirty;

	int				mWidth;
	int				mHeight;
	ci::vec2		mWindowSize;

	float			mBrightness;

	unsigned		mSelected;

	//! Determines the number of horizontal and vertical control points
	int				mControlsX;
	int				mControlsY;

	std::vector<ci::vec2>	mPoints;

private:
	//! edit mode for all warps
	static std::atomic<bool>		sIsEditMode;
	//! time of last control point selection
	static std::atomic<double>		sSelectedTime;
	//! keep track of mouse position
	static std::atomic<ci::ivec2>	sMouse;

	ci::vec2			mOffset;
};

// ----------------------------------------------------------------------------------------------------------------

typedef std::shared_ptr<class WarpBilinear>	WarpBilinearRef;

class WarpBilinear
	: public Warp {
public:
	//
	static WarpBilinearRef create( const ci::gl::Fbo::Format &format = ci::gl::Fbo::Format() ) { return std::make_shared<WarpBilinear>( format ); }

public:
	WarpBilinear( const ci::gl::Fbo::Format &format = ci::gl::Fbo::Format() );
	virtual ~WarpBilinear( void );

	//! returns a shared pointer to this warp
	WarpBilinearRef	getPtr() { return std::static_pointer_cast<WarpBilinear>( shared_from_this() ); }

	//!
	virtual ci::XmlTree	toXml() const override;
	//!
	virtual void		fromXml( const ci::XmlTree &xml ) override;

	//! set the width and height of the content in pixels
	void				setSize( int w, int h ) override { Warp::setSize( w, h ); mFbo.reset(); }
	//! set the frame buffer format, so you have control over its quality settings
	void				setFormat( const ci::gl::Fbo::Format &format ) { mFboFormat = format; mFbo.reset(); }
	//!
	void				setLinear( bool enabled = true ) { mIsLinear = enabled; mIsDirty = true; };
	void				setCurved( bool enabled = true ) { mIsLinear = !enabled; mIsDirty = true; };

	//! reset control points to undistorted image
	virtual void		reset() override;
	//! setup the warp before drawing its contents
	virtual void		begin() override;
	//! restore the warp after drawing
	virtual void		end() override;

	//! draws a warped texture
	virtual void		draw( const ci::gl::Texture2dRef &texture, const ci::Area &srcArea, const ci::Rectf &destRect ) override;

	//! set the number of horizontal control points for this warp 
	void				setNumControlX( int n );
	//! set the number of vertical control points for this warp
	void				setNumControlY( int n );

	void				setTexCoords( float x1, float y1, float x2, float y2 );

	virtual void		keyDown( ci::app::KeyEvent &event ) override;
protected:
	//! draws the warp as a mesh, allowing you to use your own texture instead of the FBO
	virtual void		draw( bool controls = true ) override;
	//! Creates the shader that renders the content with a wireframe overlay
	void				createShader();
	//! Creates the frame buffer object and updates the vertex buffer object if necessary
	void				createBuffers();
	//! Creates the vertex buffer object
	void				createMesh( int resolutionX = 36, int resolutionY = 36 );
	//! Updates the vertex buffer object based on the control points
	void				updateMesh();
	//!	Returns the specified control point. Values for col and row are clamped to prevent errors.
	ci::vec2			getPoint( int col, int row ) const;
	//! Performs fast Catmull-Rom interpolation, returns the interpolated value at t
	ci::vec2			cubicInterpolate( const std::vector<ci::vec2> &knots, float t ) const;
	//!
	ci::Rectf			getMeshBounds() const;
private:
	//! Greatest common divisor using Euclidian algorithm (from: http://en.wikipedia.org/wiki/Greatest_common_divisor)
	inline int			gcd( int a, int b ) const { if( b == 0 ) return a; else return gcd( b, a%b ); };
protected:
	ci::gl::FboRef			mFbo;
	ci::gl::Fbo::Format		mFboFormat;
	ci::gl::VboMeshRef		mVboMesh;
	ci::gl::GlslProgRef		mShader;
	ci::gl::BatchRef		mBatch;

	//! linear or curved interpolation
	bool					mIsLinear;
	//!
	bool					mIsAdaptive;

	//! texture coordinates of corners
	float					mX1, mY1, mX2, mY2;

	//! Determines the detail of the generated mesh. Multiples of 5 seem to work best.
	int						mResolution;

	//! Determines the number of horizontal and vertical quads 
	int						mResolutionX;
	int						mResolutionY;
};

// ----------------------------------------------------------------------------------------------------------------

typedef std::shared_ptr<class WarpPerspective> WarpPerspectiveRef;

class WarpPerspective
	: public Warp {
public:
	//
	static WarpPerspectiveRef create() { return std::make_shared<WarpPerspective>(); }

public:
	WarpPerspective( void );
	virtual ~WarpPerspective( void );

	//! returns a shared pointer to this warp
	WarpPerspectiveRef	getPtr() { return std::static_pointer_cast<WarpPerspective>( shared_from_this() ); }

	//! get the transformation matrix
	ci::mat4		getTransform();
	//! get the inverted transformation matrix
	ci::mat4		getInvertedTransform() { return mInverted; }

	//! reset control points to undistorted image
	void			reset() override;
	//! setup the warp before drawing its contents
	void			begin() override;
	//! restore the warp after drawing
	void			end() override;

	//! draws a warped texture
	void			draw( const ci::gl::Texture2dRef &texture, const ci::Area &srcArea, const ci::Rectf &destRect ) override;

	//! override keyDown method to add additional key handling
	void			keyDown( ci::app::KeyEvent &event ) override;

	//! allow WarpPerspectiveBilinear to access the protected class members
	friend class WarpPerspectiveBilinear;
protected:
	//!
	void	draw( bool controls = true ) override;

	//! find homography based on source and destination quad
	ci::mat4 getPerspectiveTransform( const ci::vec2 src[4], const ci::vec2 dst[4] ) const;
	//! helper function
	void gaussianElimination( float * input, int n ) const;

protected:
	ci::vec2	mSource[4];
	ci::vec2	mDestination[4];

	ci::mat4	mTransform;
	ci::mat4	mInverted;
};

// ----------------------------------------------------------------------------------------------------------------

typedef std::shared_ptr<class WarpPerspectiveBilinear>	WarpPerspectiveBilinearRef;

class WarpPerspectiveBilinear
	: public WarpBilinear {
public:
	//
	static WarpPerspectiveBilinearRef create( const ci::gl::Fbo::Format &format = ci::gl::Fbo::Format() ) { return std::make_shared<WarpPerspectiveBilinear>( format ); }

public:
	WarpPerspectiveBilinear( const ci::gl::Fbo::Format &format = ci::gl::Fbo::Format() );
	virtual ~WarpPerspectiveBilinear( void );

	//! returns a shared pointer to this warp
	WarpPerspectiveBilinearRef	getPtr() { return std::static_pointer_cast<WarpPerspectiveBilinear>( shared_from_this() ); }

	//!
	ci::XmlTree	toXml() const override;
	//!
	void		fromXml( const ci::XmlTree &xml ) override;

	void		mouseMove( ci::app::MouseEvent &event ) override;
	void		mouseDown( ci::app::MouseEvent &event ) override;
	void		mouseDrag( ci::app::MouseEvent &event ) override;

	void		keyDown( ci::app::KeyEvent &event ) override;

	void		resize() override;

	//! set the width and height of the content in pixels
	void		setSize( int w, int h ) override;

	//! returns the coordinates of the specified control point
	ci::vec2	getControlPoint( unsigned index ) const override;
	//! sets the coordinates of the specified control point
	void		setControlPoint( unsigned index, const ci::vec2 &pos ) override;
	//! moves the specified control point 
	void		moveControlPoint( unsigned index, const ci::vec2 &shift ) override;
	//! select one of the control points
	void		selectControlPoint( unsigned index ) override;
	//! deselect the selected control point
	void		deselectControlPoint() override;
protected:
	//! 
	void		draw( bool controls = true ) override;

	//! returns whether or not the control point is one of the 4 corners and should be treated as a perspective control point
	bool		isCorner( unsigned index ) const;
	//! converts the control point index to the appropriate perspective warp index
	unsigned	convertIndex( unsigned index ) const;

protected:
	WarpPerspectiveRef	mWarp;
};

}
} // namespace ph::warping
