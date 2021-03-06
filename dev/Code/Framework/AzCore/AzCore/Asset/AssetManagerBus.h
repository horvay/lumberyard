/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/
#ifndef AZCORE_ASSET_DATABASE_BUS_H
#define AZCORE_ASSET_DATABASE_BUS_H

#include <AzCore/EBus/EBus.h>
#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/std/string/string.h>

namespace AZ
{
    namespace Data
    {
        /** Asset Information (returned by bus queries to the catalog)
        * Note that Multiple UUIDs may point at the same "asset information"
        * so that legacy UUIDs (such as those generated using a different scheme) can still resolve to a valid asset
        * however, only one such entry will have 'canonical' set to true, meaning its the latest scheme.
        * UIs which enumerate assets should only use canonical assets.
        */
        class AssetInfo
        {
        public:
            AZ_TYPE_INFO(AssetInfo, "{E6D8372B-8419-4287-B478-1353709A972F}");
            AZ::Data::AssetId m_assetId; // this is in case you look up by a legacy Id or other remapping and it resolves to a new ID.
            AZ::Data::AssetType m_assetType = s_invalidAssetType;
            AZ::u64 m_sizeBytes = 0;
            AZStd::string m_relativePath; // (legacy asset name)
        };

        /**
         * Request bus for asset catalogs. Presently we expect only one asset catalog, so this
         * bus is limited to one handlers.
         */
        class AssetCatalogRequests
            : public AZ::EBusTraits
        {
        public:

            //////////////////////////////////////////////////////////////////////////
            // EBusTraits overrides - Application is a singleton
            static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
            typedef AZStd::recursive_mutex MutexType;
            //////////////////////////////////////////////////////////////////////////

            virtual ~AssetCatalogRequests() = default;

            /// Enables the catalog.
            virtual void EnableCatalogForAsset(const AZ::Data::AssetType& /*assetType*/) {}

            /// Disables the catalog.
            virtual void DisableCatalog() {}

            /// Enable monitoring of asset changes.
            virtual void StartMonitoringAssets() {};

            /// Stop monitoring of asset changes.
            virtual void StopMonitoringAssets() {};

            /// Populates catalog data from specified file.
            /// \param catalogRegistryFile cache-relative file path from which catalog should be pre-loaded.
            /// \return true if catalog was successfuly loaded.
            virtual bool LoadCatalog(const char* /*catalogRegistryFile*/) { return false; };

            /// Adds an extension to the catalog's handled list.
            /// \param file extension to add to catalog's list of those handled. With and without prefix '.' are both accepted.
            virtual void AddExtension(const char* /*extension*/) {}

            /// Adds an asset type to the catalog's handled list.
            /// \param asset type to add to the catalog's list of those handled.
            virtual void AddAssetType(const AZ::Data::AssetType& /*assetType*/) {}

            /// Fills a vector with all registered AssetTypes.
            /// \param the list reference to fill with registered types.
            virtual void GetHandledAssetTypes(AZStd::vector<AZ::Data::AssetType>& /*assetTypes*/) {}

            /// Adds an asset to the catalog.
            /// \param id - the id to assign the asset. 
            /// \param info - the information to assign to that ID
            virtual void RegisterAsset(const AZ::Data::AssetId& /*id*/, AZ::Data::AssetInfo& /*info*/) {}

            /// Removes an asset from the catalog (by ID)
            virtual void UnregisterAsset(const AZ::Data::AssetId& /*id*/) {}

            /// Retrieves an asset-root-relative path by Id.
            /// \return asset relative path given an Id, if it's in the catalog, otherwise an empty string.
            virtual AZStd::string GetAssetPathById(const AZ::Data::AssetId& /*id*/) { return AZStd::string(); }

            /// Retrieves an asset Id given a full or asset-root-relative path.
            /// \param path - asset full or asset-root relative path.
            /// \param typeToRegister - if autoRegisterIfNotFound is set and the asset isn't already registered, it will be registered as this type.
            /// \param autoRegisterIfNotFound - registers the asset if not already in the catalog.
            /// \return valid AssetId if it's in the registry, otherwise an empty AssetId.
            virtual AZ::Data::AssetId GetAssetIdByPath(const char* /*path*/, const AZ::Data::AssetType& /*typeToRegister*/, bool /*autoRegisterIfNotFound*/) { return AZ::Data::AssetId(); }

            /// Given an asset ID, retrieve general information about that asset.
            virtual AZ::Data::AssetInfo GetAssetInfoById(const AZ::Data::AssetId& /*id*/) { return AssetInfo(); }

            /// Compute an asset Id from a path.
            /// This is TEMPORARY functionality. Side-by-side metadata and/or  will eventually contain Uuid information.
            /// For now it's computed based on path.
            /// \param path - asset full or asset-root relative path.
            /// \return AssetId computed from path. Returned Id will be invalid if input path is full, but not under the asset root.
            virtual AZ::Data::AssetId GenerateAssetIdTEMP(const char* /*path*/) { return AZ::Data::AssetId(); }

            using BeginAssetEnumerationCB = AZStd::function< void() >;
            using AssetEnumerationCB = AZStd::function< void(const AZ::Data::AssetId /*id*/, const AZ::Data::AssetInfo& /*info*/) >;
            using EndAssetEnumerationCB = AZStd::function< void() >;

            /// Iterate through all assets and call the callback for each one.
            /// These callbacks will run on the same thread as the caller.
            /// \param beginCB - called before any assets are enumerated.
            /// \param enumerateCB - called for each asset.
            /// \param endCB - called after all assets are enumerated.
            virtual void EnumerateAssets(BeginAssetEnumerationCB /*beginCB*/, AssetEnumerationCB /*enumerateCB*/, EndAssetEnumerationCB /*endCB*/) {}
        };

        using AssetCatalogRequestBus = AZ::EBus<AssetCatalogRequests>;

        /*
         * Events that AssetManager listens for
         */
        class AssetManagerEvents
            : public EBusTraits
        {
        public:
            //////////////////////////////////////////////////////////////////////////
            // EBusTraits overrides
            static const EBusHandlerPolicy HandlerPolicy = EBusHandlerPolicy::Single;
            typedef AZStd::recursive_mutex  MutexType;
            //////////////////////////////////////////////////////////////////////////

            virtual ~AssetManagerEvents() {}

            /// Signal that an asset is ready for use
            virtual void OnAssetReady(const Asset<AssetData>& asset) = 0;
            /// Signal that an asset has been reloaded
            virtual void OnAssetReloaded(const Asset<AssetData>& asset) = 0;
            /// Signal that an asset failed to reload.
            virtual void OnAssetReloadError(const Asset<AssetData>& asset) = 0;
            /// Signal that an asset error has occurred
            virtual void OnAssetError(const Asset<AssetData>& asset) = 0;

            /// Notify listeners that all asset events have been dispatched.
            virtual void OnAssetEventsDispatched() {};
        };
        typedef EBus<AssetManagerEvents> AssetManagerBus;

        /*
         * Events that the AssetManager broadcasts.
         */
        class AssetManagerNotifications
            : public EBusTraits
        {
        public:
            //////////////////////////////////////////////////////////////////////////
            // EBusTraits overrides
            static const EBusHandlerPolicy HandlerPolicy = EBusHandlerPolicy::Multiple;
            typedef AZStd::recursive_mutex  MutexType;
            //////////////////////////////////////////////////////////////////////////

            /// Notify listeners that all asset events have been dispatched.
            virtual void OnAssetEventsDispatched() {};
        };
        typedef EBus<AssetManagerNotifications> AssetManagerNotificationBus;

    }   // namespace Data
}   // namespace AZ

#endif  // AZCORE_ASSET_DATABASE_BUS_H
#pragma once
