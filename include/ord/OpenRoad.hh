/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2019, The Regents of the University of California
// All rights reserved.
//
// BSD 3-Clause License
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <set>
#include <string>
#include <vector>

extern "C" {
struct Tcl_Interp;
}

namespace odb {
class dbDatabase;
class dbBlock;
class dbTech;
class dbLib;
class Point;
class Rect;
}  // namespace odb

namespace sta {
class dbSta;
class dbNetwork;
class Resizer;
}  // namespace sta

namespace rsz {
class Resizer;
}

namespace ppl {
class IOPlacer;
}

namespace rmp {
class Restructure;
}

namespace cts {
class TritonCTS;
}

namespace grt {
class GlobalRouter;
}

namespace tap {
class Tapcell;
}

namespace dpl {
class Opendp;
}

namespace dpo {
class Optdp;
}

namespace fin {
class Finale;
}

namespace mpl {
class MacroPlacer;
}

namespace mpl2 {
class MacroPlacer2;
}

namespace gpl {
class Replace;
}

namespace rcx {
class Ext;
}

namespace triton_route {
class TritonRoute;
}

namespace psm {
class PDNSim;
}

namespace ant {
class AntennaChecker;
}

namespace par {
class PartitionMgr;
}

namespace pdn {
class PdnGen;
}

namespace pad {
class ICeWall;
}

namespace utl {
class Logger;
}

namespace dst {
class Distributed;
}
namespace stt {
class SteinerTreeBuilder;
}

namespace dft {
class Dft;
}

namespace ord {

using std::string;

class dbVerilogNetwork;

// Only pointers to components so the header has no dependents.
class OpenRoad
{
 public:
  // Singleton accessor.
  // This accessor should ONLY be used for tcl commands.
  // Tools should use their initialization functions to get the
  // OpenRoad object and/or any other tools they need to reference.
  static OpenRoad* openRoad();
  void init(Tcl_Interp* tcl_interp);

  Tcl_Interp* tclInterp() { return tcl_interp_; }
  utl::Logger* getLogger() { return logger_; }
  odb::dbDatabase* getDb() { return db_; }
  sta::dbSta* getSta() { return sta_; }
  sta::dbNetwork* getDbNetwork();
  rsz::Resizer* getResizer() { return resizer_; }
  rmp::Restructure* getRestructure() { return restructure_; }
  cts::TritonCTS* getTritonCts() { return tritonCts_; }
  dbVerilogNetwork* getVerilogNetwork() { return verilog_network_; }
  dpl::Opendp* getOpendp() { return opendp_; }
  dpo::Optdp* getOptdp() { return optdp_; }
  fin::Finale* getFinale() { return finale_; }
  tap::Tapcell* getTapcell() { return tapcell_; }
  mpl::MacroPlacer* getMacroPlacer() { return macro_placer_; }
  mpl2::MacroPlacer2* getMacroPlacer2() { return macro_placer2_; }
  rcx::Ext* getOpenRCX() { return extractor_; }
  triton_route::TritonRoute* getTritonRoute() { return detailed_router_; }
  gpl::Replace* getReplace() { return replace_; }
  psm::PDNSim* getPDNSim() { return pdnsim_; }
  grt::GlobalRouter* getGlobalRouter() { return global_router_; }
  par::PartitionMgr* getPartitionMgr() { return partitionMgr_; }
  ant::AntennaChecker* getAntennaChecker() { return antenna_checker_; }
  ppl::IOPlacer* getIOPlacer() { return ioPlacer_; }
  pdn::PdnGen* getPdnGen() { return pdngen_; }
  pad::ICeWall* getICeWall() { return icewall_; }
  dst::Distributed* getDistributed() { return distributer_; }
  stt::SteinerTreeBuilder* getSteinerTreeBuilder() { return stt_builder_; }
  dft::Dft* getDft() { return dft_; }

  // Return the bounding box of the db rows.
  odb::Rect getCore();
  // Return true if the command units have been initialized.
  bool unitsInitialized();

  void readLef(const char* filename,
               const char* lib_name,
               bool make_tech,
               bool make_library);

  void readDef(const char* filename,
               bool continue_on_errors,
               bool floorplan_init,
               bool incremental);

  void writeLef(const char* filename);

  void writeDef(const char* filename,
                // major.minor (avoid including defout.h)
                string version);

  void writeCdl(const char* outFilename,
                const std::vector<const char*>& mastersFilenames,
                bool includeFillers);

  void readVerilog(const char* filename);
  void linkDesign(const char* top_cell_name);

  // Used if a design is created programmatically rather than loaded
  // to notify the tools (eg dbSta, gui).
  void designCreated();

  void readDb(const char* filename);
  void writeDb(const char* filename);

  void diffDbs(const char* filename1, const char* filename2, const char* diffs);

  void setThreadCount(int threads, bool printInfo = true);
  void setThreadCount(const char* threads, bool printInfo = true);
  int getThreadCount();

  // Observer interface
  class Observer
  {
   public:
    virtual ~Observer();

    // Either pointer could be null
    virtual void postReadLef(odb::dbTech* tech, odb::dbLib* library) = 0;
    virtual void postReadDef(odb::dbBlock* block) = 0;
    virtual void postReadDb(odb::dbDatabase* db) = 0;

   private:
    OpenRoad* owner_ = nullptr;
    friend class OpenRoad;
  };

  void addObserver(Observer* observer);
  void removeObserver(Observer* observer);

 protected:
  ~OpenRoad();

 private:
  OpenRoad();

  Tcl_Interp* tcl_interp_;
  utl::Logger* logger_;
  odb::dbDatabase* db_;
  dbVerilogNetwork* verilog_network_;
  sta::dbSta* sta_;
  rsz::Resizer* resizer_;
  ppl::IOPlacer* ioPlacer_;
  dpl::Opendp* opendp_;
  dpo::Optdp* optdp_;
  fin::Finale* finale_;
  mpl::MacroPlacer* macro_placer_;
  mpl2::MacroPlacer2* macro_placer2_;
  grt::GlobalRouter* global_router_;
  rmp::Restructure* restructure_;
  cts::TritonCTS* tritonCts_;
  tap::Tapcell* tapcell_;
  rcx::Ext* extractor_;
  triton_route::TritonRoute* detailed_router_;
  ant::AntennaChecker* antenna_checker_;
  gpl::Replace* replace_;
  psm::PDNSim* pdnsim_;
  par::PartitionMgr* partitionMgr_;
  pdn::PdnGen* pdngen_;
  pad::ICeWall* icewall_;
  dst::Distributed* distributer_;
  stt::SteinerTreeBuilder* stt_builder_;
  dft::Dft* dft_;

  std::set<Observer*> observers_;

  int threads_;
};

int tclAppInit(Tcl_Interp* interp);

}  // namespace ord
