//
//  Materialization_lazy.h
//  elly
//
//  Created by Ce Zhang on 7/28/12.
//  Copyright (c) 2012 University of Wisconsin-Madison. All rights reserved.
//

#ifndef elly_Materialization_lazy_h
#define elly_Materialization_lazy_h

#include "../utils/Common.h"
#include "../utils/FactorFileParser.h"

namespace mia{
    namespace elly{
        
        namespace mat{
                       
            class Materialization_lazy{
                
            public:
                
                mia::elly::utils::FactorFileParser * parserrs;
                
                Materialization_lazy(mia::elly::utils::FactorFileParser * _parserrs){
                    
                    parserrs = _parserrs;
                    
                }
                                
                void materialize(){
                    
                    mia::elly::utils::log() << ">> Materializing for Lazy...." << std::endl;
                    
                }
                
                int getNVariable(){
                    return parserrs->va.nvariable;
                }
                
                void update(mia::elly::SampleInput & sampleInput, int newvalue){
                 
                    //char aaa[1000];
                    //sprintf(aaa, "%d from %d to %d\n", sampleInput.vid, sampleInput.vvalue, newvalue );
                    //std::cout << aaa << std::endl;
                    
                    parserrs->va.set(sampleInput.vid, newvalue);
                    
                    int crid, fid, aux, vpos, funcid;
                    void * mb;
                    
                    for(int nf=0; nf<sampleInput.fids.size();nf ++){
                        
                        crid = sampleInput.crids[nf];
                        fid = sampleInput.fids[nf];
                        aux = sampleInput.auxs[nf];
                        mb = sampleInput.mbs[nf];
                        vpos = sampleInput.pos_of_sample_variable[nf];
                        funcid = sampleInput.funcids[nf];
                        
                        if(vpos == -1){
                            //parserrs->crs[crid]->lock(fid);
                            
                            parserrs->crs[crid]->update(fid, funcs_update[funcid], sampleInput.vid, sampleInput.vvalue, newvalue);
                            
                            //parserrs->crs[crid]->release(fid);
                        }
                        
                    }
                
                    //for(int nf=0; nf<sampleInput.fids.size();nf ++){
                    //
                    //    crid = sampleInput.crids[nf];
                    //    fid = sampleInput.fids[nf];
                    //
                    //    //todo: should we lock variables?
                    //    parserrs->crs[crid]->release(fid);
                    //
                    //}
                }

                
                // get mia::elly:SampleInput object as input to sampler
                void retrieve(int vid, mia::elly::SampleInput & rs){
                    
                    
                    //assert(rs.mbs.size() == 0);
                    //assert(rs.pos_of_sample_variable.size() == 0);
                    //assert(rs.auxs.size() == 0);
                    
                    // first get a list of factors of vid
                    mia::sm::IntsBlock factors = parserrs->vf.lookup(vid);
                    
                    int crid;
                    int fid;
                    int mbvid;
                    int mbvvalue;
                    int aux;
                    
                    int currentNFactor = 0;
                    int currentNVariable = 0;
                    
                    //for(int factor=1;factor<factors.size;factor+=2){
                    //    crid = factors.get<int>(factor);
                    //    fid = factors.get<int>(factor+1);
                    //    parserrs->crs[crid]->lock(fid);
                    //}
                                        
                    rs.vid = vid;
                    rs.vvalue = parserrs->va.lookup(vid);
                    rs.vdomain = parserrs->va.lookup_domain(vid);
                    
                    // for each factor
                    for(int factor=1;factor<factors.size;factor+=2){
                        
                        crid = factors.get<int>(factor);
                        fid = factors.get<int>(factor+1);  
                        
                        rs.crids.push_back(crid);
                        rs.fids.push_back(fid);
                        
                        rs.funcids.push_back(parserrs->crs[crid]->function_id);
                        
                        // if not incremental factor
                        if(funcs_incremental[parserrs->crs[crid]->function_id] == false){
                        
                            std::vector<int> * empty = new std::vector<int>;
                            
                            //std::cout << crid << ", " << fid << std::endl;
                            mia::sm::IntsBlock * mb =
                                (mia::sm::IntsBlock*) parserrs->crs[crid]->lookup(fid);
                        
                            aux = mb->get<int>(0);
                        
                            rs.auxs.push_back(aux);
                        
                            // for each variables in that factor, fetch current values
                            currentNVariable = 0;
                        
                            //std::cout << "mb->size = " << mb->size << std::endl;
                        
                            for(int nmbv=1; nmbv < mb->size; nmbv ++){
                            
                                mbvid = mb->get<int>(nmbv);
                                mbvvalue = parserrs->va.lookup(mbvid);
                            
                                //std::cout << mbvid << " = " << mbvvalue << "mb->size = " << mb->size << std::endl;
                            
                                if(mbvid == vid){
                          
                                    //std::cout << rs.pos_of_sample_variable.size() << "~~~" << currentNFactor << std::endl;
                                
                                    assert(rs.pos_of_sample_variable.size() == currentNFactor); // variable ID is distinct in each factor! same varialbe do not appear twice.
                                
                                    rs.pos_of_sample_variable.push_back(currentNVariable);
                                
                                }
                            
                                //std::cout << "pushed_value = " << mbvvalue << std::endl;
                                empty->push_back(mbvvalue);
                            
                                currentNVariable ++;
                            
                            }
                                                
                            delete (mia::sm::IntsBlock *) mb;
                            rs.mbs.push_back((void*)empty);
                        }else{
                            
                            rs.auxs.push_back(-1);
                            rs.pos_of_sample_variable.push_back(-1);
                            void * mb = (void*) parserrs->crs[crid]->lookup(fid);
                            
                            //std::cout << mb << std::endl;
                            
                            rs.mbs.push_back(mb);
                            
                        }
                        
                        currentNFactor ++;
                    }
                    
                    //rs.print();
                    
                }
                
                
                
                
                
            };
        
        }
    }
}



#endif