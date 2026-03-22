#pragma once
#include <vector>
#include <memory>
#include "Rules.h"

namespace bamboo { namespace engine {

    struct Transformation {
        Rule rule;
        Transformation* target = nullptr; // Pointer to the transformation being modified
        bool isUpperCase = false;
        char32_t resultChar = 0; // The character resulting from this transformation
    };

    class StateManager {
    public:
        StateManager();
        ~StateManager() = default;

        void AddTransformation(const Rule& rule, Transformation* target, bool isUpperCase);
        void RemoveLastChar();
        void Reset();
        void RebuildFromText(std::u32string_view text, bool stdStyle);

        const std::vector<std::unique_ptr<Transformation>>& GetComposition() const { return composition; }
        std::string Flatten(uint32_t mode);

        // Extract last word/syllable logic
        std::pair<size_t, size_t> GetLastWordRange() const;
        bool IsValidSyllable(bool fullComplete) const;

    private:
        std::vector<std::unique_ptr<Transformation>> composition;
    };

}} // namespace bamboo::engine
