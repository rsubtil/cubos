#ifndef CUBOS_ENGINE_DATA_LOADER_HPP
#define CUBOS_ENGINE_DATA_LOADER_HPP

#include <cubos/engine/data/meta.hpp>

#include <concepts>
#include <future>

namespace cubos::engine::data
{
    class AssetManager;

    /// Base class of all asset loaders.
    class Loader
    {
    public:
        /// @param manager The asset manager that owns this loader.
        Loader(AssetManager* manager);
        virtual ~Loader() = default;

        /// Loads an asset, synchronously.
        /// @param meta Asset meta data.
        /// @returns Pointer to asset data.
        virtual const void* load(const Meta& meta) = 0;

        /// Loads an asset, asynchronously.
        /// @param meta Asset meta data.
        /// @returns Pointer to asset data.
        virtual std::future<const void*> loadAsync(const Meta& meta) = 0;

        /// Unloads an asset.
        /// @param meta Asset meta data.
        /// @param asset Pointer to asset data.
        virtual void unload(const Meta& meta, const void* asset) = 0;

    protected:
        AssetManager* manager; ///< Asset manager that owns this loader.
    };
} // namespace cubos::engine::data

#endif // CUBOS_ENGINE_DATA_LOADER_HPP
