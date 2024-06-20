// -*- C++ -*-
/*! \file
 * \brief Inline Get and write part of prop
 *
 */

#ifndef __inline_write_part_prop_w_h__
#define __inline_write_part_prop_w_h__

#include <iostream>
#include "chroma.h"

namespace Chroma
{
    /*! \ingroup inlinehadron */
    namespace InlineWritePartPropEnv
    {
        extern const std::string name;
        bool                     registerAll();
    }

    //! Parameter structure
    /*! \ingroup inlinehadron */
    struct InlineWritePartPropParams
    {
        InlineWritePartPropParams();
        InlineWritePartPropParams(XMLReader &xml_in, const std::string &path);
        void writeXML(XMLWriter &xml_out, const std::string &path);

        unsigned long frequency;
        std::string   filename;
        // multi1d<int>  srce_coord;
        multi1d<int>  sink_coord;
        int           gamma_id;

        struct NamedObject_t
        {
            // std::string gauge_id; /*!< Input gauge field */
            std::string prop_id; /*!< Input prop field */
        } named_obj;

        std::string output;
        std::string xml_file; // Alternate XML file pattern
    };


    //! Inline measurement of hadron spectrum
    /*! \ingroup inlinehadron */
    class InlineWritePartProp : public AbsInlineMeasurement
    {
      public:
        ~InlineWritePartProp() {}
        InlineWritePartProp(const InlineWritePartPropParams &p) : params(p) {}
        InlineWritePartProp(const InlineWritePartProp &p) : params(p.params) {}

        unsigned long getFrequency(void) const { return params.frequency; }

        //! Do the measurement
        void operator()(const unsigned long update_no, XMLWriter &xml_out);

      protected:
        void func(const unsigned long update_no, XMLWriter &xml_out);

      private:
        InlineWritePartPropParams params;
    };

};

#endif
