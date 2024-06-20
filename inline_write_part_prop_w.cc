/*! \file
 * \brief Inline construction of hadron spectrum
 *
 * Spectrum calculations
 */

#include <stdio.h>
#include "inline_write_part_prop_w.h"

using SpinColorMatrx = QDP::PSpinMatrix<QDP::PColorMatrix<QDP::RComplex<REAL>, 3>, 4>;

namespace Chroma
{
    namespace InlineWritePartPropEnv
    {
        namespace
        {
            AbsInlineMeasurement *createMeasurement(XMLReader &xml_in, const std::string &path) {
                return new InlineWritePartProp(InlineWritePartPropParams(xml_in, path));
            }
            bool registered = false;
        }

        const std::string name = "WRITE_PART_PROP";

        //! Register all the factories
        bool registerAll() {
            bool success = true;
            if (!registered) {
                success &= TheInlineMeasurementFactory::Instance().registerObject(name, createMeasurement);
                registered = true;
            }
            return success;
        }
    }


    void read(XMLReader &xml, const std::string &path, InlineWritePartPropParams::NamedObject_t &input) {
        XMLReader inputtop(xml, path);
        read(inputtop, "prop_id", input.prop_id);
    }

    void write(XMLWriter &xml, const std::string &path, const InlineWritePartPropParams::NamedObject_t &input) {
        push(xml, path);
        write(xml, "prop_id", input.prop_id);
        pop(xml);
    }

    InlineWritePartPropParams::InlineWritePartPropParams() { frequency = 0; }

    InlineWritePartPropParams::InlineWritePartPropParams(XMLReader &xml_in, const std::string &path) {
        try {
            XMLReader paramtop(xml_in, path);
            if (paramtop.count("Frequency") == 1) {
                read(paramtop, "Frequency", frequency);
            } else {
                frequency = 1;
            }

            read(paramtop, "filename", filename);
            // read(paramtop, "srce_coord", srce_coord);
            read(paramtop, "sink_coord", sink_coord);
            read(paramtop, "gamma_id", gamma_id);
            read(paramtop, "NamedObject", named_obj);

            if (paramtop.count("xml_file") != 0) read(paramtop, "xml_file", xml_file);
        }
        catch (const std::string &e) {
            QDPIO::cerr << __func__ << ": Caught Exception reading XML: " << e << std::endl;
            QDP_abort(1);
        }
    }

    void InlineWritePartPropParams::writeXML(XMLWriter &xml_out, const std::string &path) {
        push(xml_out, path);
        write(xml_out, "filename", filename);
        // write(xml_out, "srce_coord", srce_coord);
        write(xml_out, "gamma_id", gamma_id);
        write(xml_out, "sink_coord", sink_coord);
        write(xml_out, "NamedObject", named_obj);
        pop(xml_out);
    }

    void InlineWritePartProp::operator()(unsigned long update_no, XMLWriter &xml_out) {
        if (params.xml_file != "") {
            std::string xml_file = makeXMLFileName(params.xml_file, update_no);
            push(xml_out, "Propagator");
            write(xml_out, "update_no", update_no);
            write(xml_out, "xml_file", xml_file);
            pop(xml_out);
            XMLFileWriter xml(xml_file);
            func(update_no, xml);
        } else
            func(update_no, xml_out);
    }

    void InlineWritePartProp::func(unsigned long update_no, XMLWriter &xml_out) {
        START_CODE();
        QDPIO::cout << InlineWritePartPropEnv::name << ": save part propagator data" << std::endl;
        StopWatch snoop;
        snoop.reset();
        snoop.start();

        // QDPIO::cout << InlineWritePartPropEnv::name << ": get propagator field" << std::endl;
        LatticePropagator prop = TheNamedObjMap::Instance().getData<LatticePropagator>(params.named_obj.prop_id);

        if (Layout::primaryNode()) {
            auto  Msc   = prop.elem(Layout::linearSiteIndex(params.sink_coord));
            auto  Corr  = trace(Gamma(params.gamma_id) * Msc);
            REAL *p_Msc = &Msc.elem(0, 0).elem(0, 0).real();
            // ColorMatrixSpinMatrix Msc    = prop.elem(linidx);
            // SpinColorMatrx        Msc    = prop.elem(linidx);

            QDPIO::cout << InlineWritePartPropEnv::name << ": sink_coord = " << params.sink_coord[0] << " " << params.sink_coord[1] << " "
                        << params.sink_coord[2] << " " << params.sink_coord[3] << std::endl;
            QDPIO::cout << InlineWritePartPropEnv::name << ": trace(Gamma(" << params.gamma_id << ") * Prop(sink_coord)) = " << Corr << std::endl;
            QDPIO::cout << InlineWritePartPropEnv::name << ": write binary data " << params.named_obj.prop_id
                        << ".elem(Layout::linearSiteIndex(params.sink_coord)) to file " << params.filename << std::endl;

            FILE *fp = fopen(params.filename.c_str(), "w");
            fwrite(p_Msc, sizeof(REAL), Ns * Nc * Ns * Nc * 2, fp);
            fclose(fp);
            fp = NULL;
        }

        snoop.stop();
        QDPIO::cout << InlineWritePartPropEnv::name << ": total time = " << snoop.getTimeInSeconds() << " secs" << std::endl;
        // QDPIO::cout << InlineWritePartPropEnv::name << ": ran successfully" << std::endl;
        END_CODE();
    }
};
