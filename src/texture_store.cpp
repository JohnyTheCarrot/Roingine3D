#include "texture_store.h"

namespace engine {
    Texture const &TextureStore::get_texture(
            std::filesystem::path const &path, TextureType type
    ) {
        auto const it = store_.find(path);
        if (it == store_.end()) {
            return store_.emplace(path.string(), Texture{path, type})
                    .first->second;
        }

        return it->second;
    }
}// namespace engine
