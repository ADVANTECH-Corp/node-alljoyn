/******************************************************************************
 * Copyright AllSeen Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for any
 *    purpose with or without fee is hereby granted, provided that the above
 *    copyright notice and this permission notice appear in all copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/

package org.alljoyn.services.android.storage;

import java.util.HashMap;
import java.util.Map;
import java.util.Set;

/**
 * A {@link PropertyStoreImpl} element that represents a property.
 */
public class Property
{
    // The language for properties who don't have a language: application id,
    // etc.
    public final static String NO_LANGUAGE = "";

    // The map of language->value for the values of this property
    private final Map<String, Object> m_values = new HashMap<String, Object>(3);

    // can it be overwritten (and hence exposed by the Config service)
    private final boolean m_isWritable;

    // is it being announced by the About service
    private final boolean m_isAnnounced;

    // can it be read by About requests.
    private final boolean m_isPublic;

    // name of the property
    private final String m_name;

    public Property(String m_name, boolean isWritable, boolean isAnnounced, boolean isPublic)
    {
        super();
        this.m_isWritable = isWritable;
        this.m_isAnnounced = isAnnounced;
        this.m_isPublic = isPublic;
        this.m_name = m_name;
    }

    /**
     * can it be overwritten (and hence exposed by the Config service)
     * 
     * @return true if this property can be overwritten
     */
    public boolean isWritable()
    {
        return m_isWritable;
    }

    /**
     * is it being announced by the About service
     * 
     * @return true if being announced by the About service
     */
    public boolean isAnnounced()
    {
        return m_isAnnounced;
    }

    /**
     * can it be read by About requests.
     * 
     * @return true if it can be read by About requests.
     */
    public boolean isPublic()
    {
        return m_isPublic;
    }

    /**
     * Property name
     * 
     * @return
     */
    public String getName()
    {
        return m_name;
    }

    /**
     * Set the value for a specific language.
     * 
     * @param language
     *            if null, NO_LANGUAGE is being used.
     * @param value
     *            new value
     */
    public void setValue(String language, Object value)
    {
        if (language == null)
        {
            language = NO_LANGUAGE;
        }
        m_values.put(language, value);
    }

    /**
     * Get the property value for a specific language
     * 
     * @param language
     * @param defaultLanguage
     * @return
     */
    public Object getValue(String language, String defaultLanguage)
    {
        if (m_values.containsKey(language))
        {
            return m_values.get(language);
        }
        else if (m_values.containsKey(defaultLanguage))
        {
            return m_values.get(defaultLanguage);
        }
        else
        {
            return m_values.get(NO_LANGUAGE);
        }
    }

    /**
     * Get the languages for which this property has a value.
     * 
     * @return the set of languages for which this property has a value.
     */
    public Set<String> getLanguages()
    {
        return m_values.keySet();
    }

    /**
     * Remove a value for a specific language
     * 
     * @param language
     */
    public void remove(String language)
    {
        m_values.remove(language);
    }

}
