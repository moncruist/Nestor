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
    const icu::UnicodeString& getCaption() const;
    void setCaption(const icu::UnicodeString& caption);
    const icu::UnicodeString& getText() const;
    void setText(const icu::UnicodeString& text);

private:
    icu::UnicodeString caption;
    icu::UnicodeString text;
};

} /* namespace rss */
} /* namespace nestor */
#endif /* RSS_OBJECT_H_ */
