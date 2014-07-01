/* vim:set et ts=4 sts=4:
 *
 * ibus-libzhuyin - New Zhuyin engine based on libzhuyin for IBus
 *
 * Copyright (c) 2014 Peng Wu <alexepico@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "ZYFallbackEditor.h"
#include <assert.h>
#include "ZYSymbols.h"
#include "ZYZhuyinProperties.h"

namespace ZY {

inline gboolean
FallbackEditor::processPunct (guint keyval, guint keycode, guint modifiers)
{
    guint cmshm_modifiers = cmshm_filter (modifiers);

    if (G_UNLIKELY (keyval == IBUS_period && cmshm_modifiers == IBUS_CONTROL_MASK)) {
        m_props.toggleModeFullPunct ();
        return TRUE;
    }

    /* check ctrl, alt, hyper, supper masks */
    if (cmshm_modifiers != 0)
        return FALSE;

    /* English mode */
    if (!m_props.modeChinese ()) {

        /* Punctuation character */
        if (is_half_punct (keyval)) {
            if (G_UNLIKELY (m_props.modeFullPunct ())) {
                String punct;
                half_punct_to_full_punct (keyval, punct);
                commit (punct);
            } else {
                commit (keyval);
            }
            return TRUE;
        }

    } else {
        /* Chinese mode, handled by ZhuyinEditor or PinyinEditor. */
        assert (FALSE);
    }
    return TRUE;
}

inline gboolean
FallbackEditor::processEnglish (guint keyval, guint keycode, guint modifiers) {
    guint cmshm_modifiers = cmshm_filter (modifiers);

    /* check ctrl, alt, hyper, supper masks */
    if (cmshm_modifiers != 0)
        return FALSE;

    /* English mode */
    if (!m_props.modeChinese ()) {

        /* English character */
        if (is_half_english (keyval)) {
            if (G_UNLIKELY (m_props.modeFullEnglish ())) {
                String english;
                half_english_to_full_english (keyval, english);
                commit (english);
            } else {
                commit (keyval);
            }
            return TRUE;
        }

    } else {
        /* Chinese mode, handled by ZhuyinEditor or PinyinEditor. */
        assert (FALSE);
    }
    return TRUE;

}

gboolean
FallbackEditor::processKeyEvent (guint keyval, guint keycode, guint modifiers)
{
    gboolean retval = FALSE;

    modifiers &= (IBUS_SHIFT_MASK |
                  IBUS_CONTROL_MASK |
                  IBUS_MOD1_MASK |
                  IBUS_SUPER_MASK |
                  IBUS_HYPER_MASK |
                  IBUS_META_MASK);

    switch (keyval) {
        /* numbers */
        case IBUS_KP_0 ... IBUS_KP_9:
            keyval = keyval - IBUS_KP_0 + IBUS_0;
        case IBUS_0 ... IBUS_9:
        /* letters */
        case IBUS_a ... IBUS_z:
        case IBUS_A ... IBUS_Z:
            if (modifiers == 0) {
                retval = processEnglish (keyval, keycode, modifiers);
            }
            break;
        /* punct */
        case IBUS_exclam ... IBUS_slash:
        case IBUS_colon ... IBUS_at:
        case IBUS_bracketleft ... IBUS_quoteleft:
        case IBUS_braceleft ... IBUS_asciitilde:
            retval = processPunct (keyval, keycode, modifiers);
            break;
        case IBUS_KP_Equal:
            retval = processPunct ('=', keycode, modifiers);
            break;
        case IBUS_KP_Multiply:
            retval = processPunct ('*', keycode, modifiers);
            break;
        case IBUS_KP_Add:
            retval = processPunct ('+', keycode, modifiers);
            break;
        #if 0
        case IBUS_KP_Separator:
            retval = processPunct (IBUS_separator, keycode, modifiers);
            break;
        #endif
        case IBUS_KP_Subtract:
            retval = processPunct ('-', keycode, modifiers);
            break;
        case IBUS_KP_Decimal:
            retval = processPunct ('.', keycode, modifiers);
            break;
        case IBUS_KP_Divide:
            retval = processPunct ('/', keycode, modifiers);
            break;
        /* space */
        case IBUS_KP_Space:
            keyval = IBUS_space;
        case IBUS_space:
            retval = processEnglish (keyval, keycode, modifiers);
            break;
        /* enter */
        case IBUS_Return:
        case IBUS_KP_Enter:
            {
                Text text ("\n");
                commitText (text);
                retval = TRUE;
            }
            break;
        /* others */
        default:
            break;
    }

    return retval;
}

void
FallbackEditor::reset (void) {
    m_quote = TRUE;
    m_double_quote = TRUE;
    m_prev_committed_char = 0;
}

};