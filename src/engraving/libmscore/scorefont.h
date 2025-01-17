/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore BVBA and others
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef MS_SCOREFONT_H
#define MS_SCOREFONT_H

#include <unordered_map>

#include "style/style.h"

#include "infrastructure/draw/geometry.h"

#include "modularity/ioc.h"
#include "infrastructure/draw/ifontprovider.h"

namespace mu {
class JsonObject;
}

namespace mu::draw {
class Painter;
}

namespace mu::engraving {
class ScoreFont
{
    INJECT_STATIC(score, mu::draw::IFontProvider, fontProvider)

public:
    ScoreFont(const char* name, const char* family, const char* path, const char* filename);
    ScoreFont(const ScoreFont& other);

    const String& name() const;
    const String& family() const;
    const String& fontPath() const;

    std::unordered_map<Sid, PropertyValue> engravingDefaults();
    double textEnclosureThickness();

    static void initScoreFonts();
    static const std::vector<ScoreFont>& scoreFonts();
    static ScoreFont* fontByName(const String& name);
    static ScoreFont* fallbackFont();
    static const char* fallbackTextFont();

    uint symCode(SymId id) const;
    SymId fromCode(uint code) const;
    String toString(SymId id) const;

    bool isValid(SymId id) const;
    bool useFallbackFont(SymId id) const;

    const mu::RectF bbox(SymId id, double mag) const;
    const mu::RectF bbox(SymId id, const mu::SizeF&) const;
    const mu::RectF bbox(const SymIdList& s, double mag) const;
    const mu::RectF bbox(const SymIdList& s, const mu::SizeF& mag) const;

    double width(SymId id, double mag) const;
    double height(SymId id, double mag) const;
    double advance(SymId id, double mag) const;
    double width(const SymIdList&, double mag) const;

    mu::PointF smuflAnchor(SymId symId, SmuflAnchorId anchorId, double mag) const;

    void draw(SymId id,         mu::draw::Painter*, double mag,            const mu::PointF& pos) const;
    void draw(SymId id,         mu::draw::Painter*, const mu::SizeF& mag, const mu::PointF& pos) const;
    void draw(SymId id,         mu::draw::Painter*, double mag,            const mu::PointF& pos, int n) const;
    void draw(const SymIdList&, mu::draw::Painter*, double mag,            const mu::PointF& pos) const;
    void draw(const SymIdList&, mu::draw::Painter*, const mu::SizeF& mag, const mu::PointF& pos) const;

private:

    struct Code {
        uint smuflCode = 0;
        uint musicSymBlockCode = 0;
    };

    struct Sym {
        uint code;
        mu::RectF bbox;
        double advance = 0.0;

        std::map<SmuflAnchorId, mu::PointF> smuflAnchors;
        SymIdList subSymbolIds;

        bool isValid() const
        {
            return code != 0 && bbox.isValid();
        }

        bool isCompound() const
        {
            return !subSymbolIds.empty();
        }
    };

    static bool initGlyphNamesJson();

    void load();
    void loadGlyphsWithAnchors(const JsonObject& glyphsWithAnchors);
    void loadComposedGlyphs();
    void loadStylisticAlternates(const JsonObject& glyphsWithAlternatesObject);
    void loadEngravingDefaults(const JsonObject& engravingDefaultsObject);
    void computeMetrics(Sym& sym, const Code& code);

    Sym& sym(SymId id);
    const Sym& sym(SymId id) const;

    bool m_loaded = false;
    std::vector<Sym> m_symbols;
    mutable mu::draw::Font m_font;

    String m_name;
    String m_family;
    String m_fontPath;
    String m_filename;

    std::unordered_map<Sid, PropertyValue> m_engravingDefaults;
    double m_textEnclosureThickness = 0;

    static std::vector<ScoreFont> s_scoreFonts;
    static std::array<Code, size_t(SymId::lastSym) + 1> s_symIdCodes;
};
}

#endif // MS_SCOREFONT_H
