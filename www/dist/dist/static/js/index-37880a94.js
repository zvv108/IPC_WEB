import{h as y,d as U,r as i,u as d,a as m,o as t,c as r,w as s,b as f,e as o,F as b,G as _,H as F,f as N,g as G,m as M,I as w,J as P,_ as H}from"./index-98552006.js";import{u as W}from"./useNav-2cc20504.js";const J=()=>y.requestWithSimpleHandle("get","/cgi-bin/entry.cgi/video"),K=g=>y.requestWithSimpleHandle("post","/cgi-bin/entry.cgi/video",{data:g}),B=g=>(w("data-v-ac0b0c58"),g=g(),P(),g),j={class:"page-content"},z=B(()=>f("span",{class:"append"},null,-1)),Q=B(()=>f("span",{class:"append"},null,-1)),X=B(()=>f("span",{class:"append"},"fps",-1)),Y=B(()=>f("span",{class:"append"},null,-1)),Z=B(()=>f("span",{class:"append"},"Kbps",-1)),$=B(()=>f("span",{class:"append"},null,-1)),ee=U({__name:"index",setup(g){const{isAdmin:x}=W(),E=i(),C=i([{value:"fixedCoderRte",label:"\u5B9A\u7801\u7387"},{value:"variableCoderRte",label:"\u53D8\u7801\u7387"}]),h=i([]),R=i([]),V=i([]),k=i([{value:"25",label:"25"},{value:"1",label:"1"},{value:"2",label:"2"},{value:"4",label:"4"},{value:"6",label:"6"},{value:"8",label:"8"},{value:"10",label:"10"},{value:"12",label:"12"},{value:"15",label:"15"},{value:"16",label:"16"},{value:"18",label:"18"},{value:"20",label:"20"},{value:"22",label:"22"}]),O=i([{value:"256",label:"256"},{value:"512",label:"512"},{value:"1024",label:"1024"},{value:"2048",label:"2048"},{value:"3072",label:"3072"},{value:"4096",label:"4096"},{value:"6144",label:"6144"},{value:"8192",label:"8192"},{value:"12288",label:"12288"},{value:"16384",label:"16384"}]),q=i({id:[{required:!0,message:"\u8BF7\u9009\u62E9\u901A\u9053\u53F7",trigger:"change"}],sStreamType:[{required:!0,message:"\u8BF7\u9009\u62E9\u7801\u6D41\u7C7B\u578B",trigger:"change"}],sResolution:[{required:!0,message:"\u8BF7\u9009\u62E9\u5206\u8FA8\u7387",trigger:"change"}],isFrameRate:[{required:!0,message:"\u8BF7\u9009\u62E9\u5E27\u7387",trigger:"change"}],sOutputDataType:[{required:!0,message:"\u8BF7\u9009\u62E9\u7801\u7387\u7C7B\u578B",trigger:"change"}],iMaxRate:[{required:!0,message:"\u8BF7\u9009\u62E9\u7801\u7387\u4E0A\u9650",trigger:"change"}],iGOP:[{required:!0,message:"\u8BF7\u8F93\u51651~400\u7684I\u5E27\u95F4\u9694",trigger:"change"},{type:"number",message:"\u8BF7\u8F93\u51651~400\u7684\u6570\u5B57",trigger:"change"},{validator:(v,l,n)=>{Number(l)<1||Number(l)>400?n(new Error("\u8BF7\u8F93\u51651~400\u7684I\u5E27\u95F4\u9694")):n()},trigger:"change"}]}),a=i([]),u=i({id:0,iGOP:void 0,iMaxRate:void 0,isFrameRate:void 0,sOutputDataType:void 0,sResolution:void 0,sStreamType:void 0,sResolutionList:[]});J().then(v=>{a.value=v.data,a.value.length>0&&(u.value={...d(a)[0]},h.value=a.value[0].sResolutionList),a.value.length>1&&(R.value=a.value[1].sResolutionList),a.value.length>2&&(V.value=a.value[2].sResolutionList)});function A(v){v==0?a.value.length>0&&(u.value={...d(a)[0]}):v==1?a.value.length>1&&(u.value={...d(a)[1]}):a.value.length>2&&(u.value={...d(a)[2]})}function D(){E.value.validate((v,l)=>{v&&(u.value.id==1?u.value.sStreamType="subStream":u.value.sStreamType="mainStream",K(u.value).then(n=>{d(u).id==0?a.value[0]={...d(u)}:d(u).id==1?a.value[1]={...d(u)}:a.value[2]={...d(u)},M("\u8BBE\u7F6E\u6210\u529F",{type:"success"})}))})}return(v,l)=>{const n=m("el-option"),p=m("el-select"),c=m("el-form-item"),L=m("el-input"),S=m("el-button"),T=m("el-form"),I=m("el-card");return t(),r(I,{class:"page-box"},{default:s(()=>[f("div",j,[o(T,{ref_key:"formRef",ref:E,model:u.value,rules:q.value,"label-width":"120","hide-required-asterisk":""},{default:s(()=>[o(c,{label:"\u901A\u9053\u53F7",prop:"id"},{default:s(()=>[o(p,{class:"flex-1",modelValue:u.value.id,"onUpdate:modelValue":l[0]||(l[0]=e=>u.value.id=e),placeholder:"\u8BF7\u9009\u62E9",onChange:A},{default:s(()=>[o(n,{label:"\u53EF\u89C1\u5149\u901A\u9053-\u4E3B\u7801\u6D41",value:0}),o(n,{label:"\u53EF\u89C1\u5149\u901A\u9053-\u5B50\u7801\u6D41",value:1}),o(n,{label:"\u7EA2\u5916\u901A\u9053-\u4E3B\u7801\u6D41",value:2})]),_:1},8,["modelValue"]),z]),_:1}),o(c,{label:"\u5206\u8FA8\u7387",prop:"sResolution"},{default:s(()=>[u.value.id==0?(t(),r(p,{key:0,class:"flex-1",modelValue:u.value.sResolution,"onUpdate:modelValue":l[1]||(l[1]=e=>u.value.sResolution=e),placeholder:"\u8BF7\u9009\u62E9"},{default:s(()=>[(t(!0),b(F,null,_(h.value,e=>(t(),r(n,{key:e,label:e,value:e},null,8,["label","value"]))),128))]),_:1},8,["modelValue"])):u.value.id==1?(t(),r(p,{key:1,class:"flex-1",modelValue:u.value.sResolution,"onUpdate:modelValue":l[2]||(l[2]=e=>u.value.sResolution=e),placeholder:"\u8BF7\u9009\u62E9"},{default:s(()=>[(t(!0),b(F,null,_(R.value,e=>(t(),r(n,{key:e,label:e,value:e},null,8,["label","value"]))),128))]),_:1},8,["modelValue"])):(t(),r(p,{key:2,class:"flex-1",modelValue:u.value.sResolution,"onUpdate:modelValue":l[3]||(l[3]=e=>u.value.sResolution=e),placeholder:"\u8BF7\u9009\u62E9"},{default:s(()=>[(t(!0),b(F,null,_(V.value,e=>(t(),r(n,{key:e,label:e,value:e},null,8,["label","value"]))),128))]),_:1},8,["modelValue"])),Q]),_:1}),o(c,{label:"\u5E27\u7387",prop:"isFrameRate"},{default:s(()=>[o(p,{class:"flex-1",modelValue:u.value.isFrameRate,"onUpdate:modelValue":l[4]||(l[4]=e=>u.value.isFrameRate=e),placeholder:"\u8BF7\u9009\u62E9"},{default:s(()=>[(t(!0),b(F,null,_(k.value,e=>(t(),r(n,{key:e.value,label:e.label,value:e.value},null,8,["label","value"]))),128))]),_:1},8,["modelValue"]),X]),_:1}),o(c,{label:"\u7801\u7387\u7C7B\u578B",prop:"sOutputDataType",rules:{required:!0,message:"\u8BF7\u9009\u62E9\u7801\u7387\u7C7B\u578B",trigger:"change"}},{default:s(()=>[o(p,{class:"flex-1",modelValue:u.value.sOutputDataType,"onUpdate:modelValue":l[5]||(l[5]=e=>u.value.sOutputDataType=e),placeholder:"\u8BF7\u9009\u62E9"},{default:s(()=>[(t(!0),b(F,null,_(C.value,e=>(t(),r(n,{key:e.value,label:e.label,value:e.value},null,8,["label","value"]))),128))]),_:1},8,["modelValue"]),Y]),_:1}),o(c,{label:"\u7801\u7387\u4E0A\u9650",prop:"iMaxRate",rules:{required:!0,message:"\u8BF7\u9009\u62E9\u7801\u7387\u4E0A\u9650",trigger:"change"}},{default:s(()=>[o(p,{class:"flex-1",modelValue:u.value.iMaxRate,"onUpdate:modelValue":l[6]||(l[6]=e=>u.value.iMaxRate=e),placeholder:"\u8BF7\u9009\u62E9"},{default:s(()=>[(t(!0),b(F,null,_(O.value,e=>(t(),r(n,{key:e.value,label:e.label,value:e.value},null,8,["label","value"]))),128))]),_:1},8,["modelValue"]),Z]),_:1}),o(c,{label:"I\u5E27\u95F4\u9694",prop:"iGOP",rules:{required:!0,message:"\u8BF7\u8F93\u5165\u5E27\u95F4\u9694",trigger:"change"}},{default:s(()=>[o(L,{class:"flex-1",modelValue:u.value.iGOP,"onUpdate:modelValue":l[7]||(l[7]=e=>u.value.iGOP=e),modelModifiers:{number:!0},placeholder:"\u8BF7\u8F93\u5165\u5E27\u95F4\u9694"},null,8,["modelValue"]),$]),_:1}),o(c,null,{default:s(()=>[d(x)?(t(),r(S,{key:0,type:"primary",onClick:D},{default:s(()=>[N(" \u4FDD\u5B58 ")]),_:1})):G("",!0)]),_:1})]),_:1},8,["model","rules"])])]),_:1})}}});const ae=H(ee,[["__scopeId","data-v-ac0b0c58"]]);export{ae as default};