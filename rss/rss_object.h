/*
 * rss_object.h
 *
 *  Created on: 14 дек. 2013 г.
 *      Author: moncruist
 */

#ifndef RSS_OBJECT_H_
#define RSS_OBJECT_H_

#include <unicode/unistr.h>

namespace nestor {
namespace rss {

class RssObject {
public:
    RssObject();
    virtual ~RssObject();
    const icu::UnicodeString& caption() const;
    void setCaption(const icu::UnicodeString& caption);
    const icu::UnicodeString& text() const;
    void setText(const icu::UnicodeString& text);
    const icu::UnicodeString& link() const;
    void setLink(const icu::UnicodeString& link);

private:
    icu::UnicodeString _caption;
    icu::UnicodeString _text;
    icu::UnicodeString _link;
};

} /* namespace rss */
} /* namespace nestor */
#endif /* RSS_OBJECT_H_ */
