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

            if (paramtop.count("gamma_id") == 1) {
                read(paramtop, "gamma_id", gamma_id);
            } else {
                gamma_id.resize(0);
            }

            read(paramtop, "file_name", file_name);
            read(paramtop, "sink_coord", sink_coord);
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
        if (gamma_id.size() > 0) { write(xml_out, "gamma_id", gamma_id); }
        write(xml_out, "file_name", file_name);
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
        QDPIO::cout << InlineWritePartPropEnv::name << ": save SpinColorMatrix of propagator " << params.named_obj.prop_id << std::endl;
        StopWatch snoop;
        snoop.reset();
        snoop.start();

        LatticePropagator lattProp = TheNamedObjMap::Instance().getData<LatticePropagator>(params.named_obj.prop_id);
        // LatticeComplex    lattCorr = trace(Gamma(params.gamma_id) * lattProp);

        int linearIdx = Layout::linearSiteIndex(params.sink_coord);
        QDPIO::cout << InlineWritePartPropEnv::name << ": sink_coord      = ";
        for (int i = 0; i < Nd; i++) { QDPIO::cout << params.sink_coord[i] << " "; }
        QDPIO::cout << std::endl;
        QDPIO::cout << InlineWritePartPropEnv::name << ": linearSiteIndex = " << linearIdx << "  (local)" << std::endl;
        QDPIO::cout << InlineWritePartPropEnv::name << ": nodeNumber      = " << Layout::nodeNumber(params.sink_coord) << std::endl;

        if (Layout::nodeNumber() == Layout::nodeNumber(params.sink_coord)) {
            auto &Msc   = lattProp.elem(linearIdx);
            auto *p_Msc = &Msc.elem(0, 0).elem(0, 0).real();
            // auto *p_Msc = &lattProp.elem(linearIdx).elem(0, 0).elem(0, 0).real(); ///< REAL for QDPXX, QDP::Word<REAL> for QDP-JITs

#ifdef WRITE_DEBUG
            std::cout << "(SpinColorMatrix) Msc:" << std::endl;
            for (int s0 = 0; s0 < Ns; s0++) {
                for (int s1 = 0; s1 < Ns; s1++) {
                    std::cout << " (ColorMatrix) Msc.elem(" << s0 << "," << s1 << ") =" << std::endl;
                    for (int c0 = 0; c0 < Nc; c0++) {
                        for (int c1 = 0; c1 < Nc; c1++) { std::cout << "  " << Msc.elem(s0, s1).elem(c0, c1); };
                        std::cout << std::endl;
                    }
                    std::cout << std::endl;
                }
            }
            std::cout << std::endl;
#endif
            if (!params.file_name.empty()) {
                FILE *fp = fopen(params.file_name.c_str(), "w");
                fwrite(p_Msc, sizeof(REAL), Ns * Nc * Ns * Nc * 2, fp);
                fclose(fp);
                fp = NULL;
                std::cout << InlineWritePartPropEnv::name << ": write to file " << params.file_name << std::endl;
            }

            auto CorrUNIT = trace(Msc);
            std::cout << InlineWritePartPropEnv::name << ": trace(Msc) = " << CorrUNIT << std::endl;

            if (params.gamma_id.size() > 0) {
                std::cout << InlineWritePartPropEnv::name << ": trace(Gamma * Msc) for gamma_id = ";
                for (int i = 0; i < params.gamma_id.size(); i++) { std::cout << params.gamma_id[i] << " "; }
                std::cout << std::endl;

                multi1d<decltype(CorrUNIT)> Corr;
                Corr.resize(params.gamma_id.size());
                for (int i = 0; i < Corr.size(); i++) { Corr[i] = trace(Gamma(params.gamma_id[i]) * Msc); }

                std::cout << InlineWritePartPropEnv::name << ": traceValues = ";
                for (int i = 0; i < Corr.size(); i++) { std::cout << Corr[i] << " "; }
                std::cout << std::endl;
            } else {
            }
        }
        // QDPIO::cout << InlineWritePartPropEnv::name << ": trace(Gamma(" << params.gamma_id << ") * Msc) = " << lattCorr.elem(linearIdx) << std::endl;

        snoop.stop();
        QDPIO::cout << InlineWritePartPropEnv::name << ": total time = " << snoop.getTimeInSeconds() << " secs" << std::endl;
        QDPIO::cout << InlineWritePartPropEnv::name << ": ran successfully" << std::endl;
        END_CODE();
    }
};
