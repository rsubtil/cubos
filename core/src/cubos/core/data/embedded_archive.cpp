#include <cubos/core/data/embedded_archive.hpp>
#include <cubos/core/data/file_stream.hpp>
#include <cubos/core/memory/buffer_stream.hpp>
#include <cubos/core/log.hpp>

using namespace cubos::core;
using namespace cubos::core::data;

EmbeddedArchive::EmbeddedArchive(const std::string& name)
{
    auto& registry = EmbeddedArchive::getRegistry();
    auto it = registry.find(name);
    if (it == registry.end())
    {
        CUBOS_CRITICAL("No embedded archive with name '{}' found", name);
        abort();
    }

    data = &it->second;
}

std::map<std::string, const EmbeddedArchive::Data&>& EmbeddedArchive::getRegistry()
{
    static std::map<std::string, const EmbeddedArchive::Data&> registry;
    return registry;
}

void EmbeddedArchive::registerData(const std::string& name, const Data& data)
{
    auto& registry = EmbeddedArchive::getRegistry();
    if (registry.find(name) != registry.end())
    {
        CUBOS_CRITICAL("Embedded archive with name '{}' already registered", name);
        abort();
    }

    registry.emplace(name, data);
}

size_t EmbeddedArchive::create(size_t parent, std::string_view name, bool directory)
{
    // Embedded archive is read-only.
    return 0;
}

bool EmbeddedArchive::destroy(size_t id)
{
    // Embedded archive is read-only.
    return false;
}

std::string EmbeddedArchive::getName(size_t id) const
{
    return this->data->entries[id - 1].name;
}

bool EmbeddedArchive::isDirectory(size_t id) const
{
    return this->data->entries[id - 1].isDirectory;
}

bool EmbeddedArchive::isReadOnly() const
{
    return true;
}

size_t EmbeddedArchive::getParent(size_t id) const
{
    return this->data->entries[id - 1].parent;
}

size_t EmbeddedArchive::getSibling(size_t id) const
{
    return this->data->entries[id - 1].sibling;
}

size_t EmbeddedArchive::getChild(size_t id) const
{
    return this->data->entries[id - 1].child;
}

std::unique_ptr<memory::Stream> EmbeddedArchive::open(File::Handle file, File::OpenMode mode)
{
    if (mode != File::OpenMode::Read)
        return nullptr;

    auto& entry = this->data->entries[file->getId() - 1];
    return std::make_unique<FileStream<memory::BufferStream>>(file, mode,
                                                              std::move(memory::BufferStream(entry.data, entry.size)));
}
