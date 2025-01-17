#include <cubos/core/gl/palette.hpp>

using namespace cubos::core::gl;

Palette::Palette(std::vector<Material>&& materials) : materials(std::move(materials))
{
}

const Material* Palette::getData() const
{
    return materials.data();
}

uint16_t Palette::getSize() const
{
    return static_cast<uint16_t>(materials.size());
}

const Material& Palette::get(uint16_t index) const
{
    if (index == 0 || index > static_cast<uint16_t>(materials.size()))
        return Material::Empty;
    return materials[index - 1];
}

void Palette::set(uint16_t index, const Material& material)
{
    if (index == 0)
    {
        CUBOS_ERROR("Cannot set a palette material at index 0: reserved for empty voxels");
        return;
    }
    else if (index > static_cast<uint16_t>(materials.size()))
        materials.resize(index, Material::Empty);

    materials[index - 1] = material;
}

uint16_t Palette::find(const Material& material) const
{
    uint16_t best_i = 0;
    float best_s = material.similarity(Material::Empty);

    for (uint16_t i = 0; i < this->getSize(); ++i)
    {
        float s = material.similarity(this->get(i + 1));
        if (s > best_s)
        {
            best_s = s;
            best_i = i + 1;
        }
    }

    return best_i;
}

uint16_t Palette::add(const Material& material, float similarity)
{
    size_t i = this->find(material);
    if (this->get(i).similarity(material) >= similarity)
    {
        return i;
    }
    else
    {
        for (uint16_t i = 0; i < this->getSize(); ++i)
        {
            if (this->get(i + 1).similarity(Material::Empty) >= 1.0f)
            {
                this->set(i + 1, material);
                return i + 1;
            }
        }

        if (this->getSize() == UINT16_MAX)
        {
            CUBOS_ERROR("Cannot add new material: palette is full");
            return i;
        }
        else
        {
            this->materials.push_back(material);
            return this->getSize();
        }
    }
}

void Palette::merge(const Palette& palette, float similarity)
{
    for (uint16_t i = 0; i < palette.getSize(); ++i)
    {
        this->add(palette.get(i + 1), similarity);
    }
}

void cubos::core::data::serialize(Serializer& serializer, const gl::Palette& palette, const char* name)
{
    // Count non-empty materials.
    size_t count = 0;
    for (const auto& material : palette.materials)
        if (memcmp(&material, &Material::Empty, sizeof(Material)) != 0)
            count++;

    serializer.beginDictionary(count, name);
    for (uint16_t i = 0; i < static_cast<uint16_t>(palette.materials.size()); i++)
        if (memcmp(&palette.materials[i], &Material::Empty, sizeof(Material)) != 0)
        {
            serializer.write<uint16_t>(i + 1, nullptr);
            serializer.write(palette.materials[i], nullptr);
        }
    serializer.endDictionary();
}

void cubos::core::data::deserialize(Deserializer& deserializer, gl::Palette& palette)
{
    palette.materials.clear();

    Material mat;
    uint16_t index;

    size_t count = deserializer.beginDictionary();
    for (size_t i = 0; i < count; i++)
    {
        deserializer.read<uint16_t>(index);
        deserializer.read(mat);

        if (index == 0)
        {
            CUBOS_WARN("Invalid palette material index 0: reserved for empty voxels");
            continue;
        }
        else if (index > palette.materials.size())
            palette.materials.resize(index, Material::Empty);
        palette.materials[index - 1] = mat;
    }
}
