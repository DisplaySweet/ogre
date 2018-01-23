/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2017 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#ifndef _OgreSceneFormatImporter_H_
#define _OgreSceneFormatImporter_H_

#include "OgreSceneFormatBase.h"
#include "OgreHeaderPrefix.h"

// Forward declaration for |Document|.
namespace rapidjson
{
    class CrtAllocator;
    template <typename> class MemoryPoolAllocator;
    template <typename> struct UTF8;
    //template <typename, typename, typename> class GenericDocument;
    //typedef GenericDocument< UTF8<char>, MemoryPoolAllocator<CrtAllocator>, CrtAllocator > Document;

    template <typename BaseAllocator> class MemoryPoolAllocator;
    template <typename Encoding, typename>  class GenericValue;
    typedef GenericValue<UTF8<char>, MemoryPoolAllocator<CrtAllocator> > Value;
}

namespace Ogre
{
    class LwString;
    class InstantRadiosity;
    class IrradianceVolume;
    class HlmsPbs;

    /** \addtogroup Component
    *  @{
    */
    /** \addtogroup Scene
    *  @{
    */
    /**
    */
    class _OgreSceneFormatExport SceneFormatImporter : public SceneFormatBase
    {
    protected:
        String mFilename;
        InstantRadiosity *mInstantRadiosity;
        IrradianceVolume *mIrradianceVolume;

        LightArray mVplLights;

        typedef map<uint32, SceneNode*>::type IndexToSceneNodeMap;
        IndexToSceneNodeMap mCreatedSceneNodes;

        void destroyInstantRadiosity(void);

        HlmsPbs* getPbs(void) const;

        static inline Light::LightTypes parseLightType( const char *value );
        static inline float decodeFloat( const rapidjson::Value &jsonValue );
        static inline double decodeDouble( const rapidjson::Value &jsonValue );
        static inline Vector2 decodeVector2Array( const rapidjson::Value &jsonArray );
        static inline Vector3 decodeVector3Array( const rapidjson::Value &jsonArray );
        static inline Vector4 decodeVector4Array( const rapidjson::Value &jsonArray );
        static inline Quaternion decodeQuaternionArray( const rapidjson::Value &jsonArray );
        static inline ColourValue decodeColourValueArray( const rapidjson::Value &jsonArray );
        static inline Aabb decodeAabbArray( const rapidjson::Value &jsonArray,
                                            const Aabb &defaultValue );

        void importNode( const rapidjson::Value &nodeValue, Node *node );
        SceneNode* importSceneNode( const rapidjson::Value &sceneNodeValue, uint32 nodeIdx,
                                    const rapidjson::Value &sceneNodesJson );
        void importSceneNodes( const rapidjson::Value &json );
        void importMovableObject( const rapidjson::Value &movableObjectValue,
                                  MovableObject *movableObject );
        void importRenderable( const rapidjson::Value &renderableValue, Renderable *renderable );
        void importSubItem( const rapidjson::Value &subItemValue, SubItem *subItem );
        void importSubEntity( const rapidjson::Value &subEntityValue, v1::SubEntity *subEntity );
        void importItem( const rapidjson::Value &itemValue );
        void importItems( const rapidjson::Value &json );
        void importEntity( const rapidjson::Value &entityValue );
        void importEntities( const rapidjson::Value &json );
        void importLight( const rapidjson::Value &lightValue );
        void importLights( const rapidjson::Value &json );
        void importInstantRadiosity( const rapidjson::Value &irValue );
        void importSceneSettings( const rapidjson::Value &json, uint32 importFlags );

    public:
        SceneFormatImporter( Root *root, SceneManager *sceneManager );
        ~SceneFormatImporter();

        /**
        @param outJson
        @param exportFlags
            Combination of SceneFlags::SceneFlags, to know what to export and what to exclude.
            Defaults to importing everything.
            Note that some combinations can cause issues:
                * Excluding scene nodes
                * Excluding meshes without excluding Items and Entities.
                * etc

            By default LightsVpl is not set so that InstantRadiosity is regenerated.
            By setting LightsVpl and unsetting SceneFlags::BuildInstantRadiosity, you can speed up
            import time because the cached results will be loaded instead.
        */
        void importScene( const String &filename, const char *jsonString,
                          uint32 importFlags=~SceneFlags::LightsVpl );

        void importSceneFromFile( const String &filename, uint32 importFlags=~SceneFlags::LightsVpl );

        /** Retrieve the InstantRadiosity pointer that may have been created while importing a scene
        @param releaseOwnership
            If true, we will return the InstantRadiosity & IrradianceVolume pointers and
            release ownership. Meaning further calls to this function will return null and
            you will be responsible for deleting it (otherwise it will leak).

            If false, we will return the InstantRadiosity & IrradianceVolume pointers but
            retain ownership. Meaning further calls to this function will still return the pointer,
            and we will delete the pointer when 'this' is destroyed, or if a new scene is imported.
        @param outInstantRadiosity [out]
            InstantRadiosity pointer. Input cannot be null. Output *outInstantRadiosity may be null
            May be null if the imported scene didn't have IR enabled,
            or if the ownership has already been released.
        @param outIrradianceVolume [out]
            IrradianceVolume pointer. Input cannot be null. Output *outIrradianceVolume may be null.
            May be null if the imported scene didn't have IR/IV enabled,
            or if the ownership has already been released.
        @return
            InstantRadiosity pointer.
        */
        void getInstantRadiosity( bool releaseOwnership,
                                  InstantRadiosity **outInstantRadiosity,
                                  IrradianceVolume **outIrradianceVolume );
    };

    /** @} */
    /** @} */

}

#include "OgreHeaderSuffix.h"

#endif