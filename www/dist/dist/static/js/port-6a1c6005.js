import{j as S,k as E}from"./system-b297d58e.js";import{u as g}from"./useNav-2cc20504.js";import{d as R,r as m,a as s,o as i,c as p,w as t,b as A,e as u,u as c,f as D,g as V,m as C,K as _,_ as h}from"./index-98552006.js";const b={class:"page-content"},x=R({__name:"port",setup(H){const{isAdmin:f}=g(),F=m(),e=m({HTTP:void 0,RTSP:void 0,SDKSERVER:void 0});let d={HTTP:void 0,RTSP:void 0,SDKSERVER:void 0};S().then(l=>{e.value=l.data,d={...l.data}});function T(){F.value.validate((l,o)=>{l&&(c(e).HTTP==d.HTTP?E(e.value).then(a=>{d={...c(e)},C("\u8BBE\u7F6E\u6210\u529F",{type:"success"})}):_.confirm("\u4FEE\u6539HTTP\u7AEF\u53E3\u5C06\u4F1A\u91CD\u542F\u7CFB\u7EDF\uFF0C\u91CD\u542F\u5927\u7EA62~4\u5206\u949F\u540E\u624D\u53EF\u767B\u5F55\uFF0C\u786E\u5B9A\u8981\u4FEE\u6539\u5417\uFF1F","\u63D0\u793A",{confirmButtonText:"\u786E\u5B9A",cancelButtonText:"\u53D6\u6D88",type:"warning"}).then(()=>{E(e.value).then(a=>{_.confirm("\u4FEE\u6539\u6210\u529F\uFF0C\u8BF72~4\u5206\u949F\u540E\u8BBF\u95EE\uFF01","\u63D0\u793A",{confirmButtonText:"\u786E\u5B9A",showCancelButton:!1,showClose:!1,closeOnClickModal:!1,type:"success"}).then(()=>{location.reload()})})}))})}return(l,o)=>{const a=s("el-input"),r=s("el-form-item"),B=s("el-button"),v=s("el-form"),P=s("el-card");return i(),p(P,{class:"page-box"},{default:t(()=>[A("div",b,[u(v,{ref_key:"formRef",ref:F,model:e.value,"label-width":"120","hide-required-asterisk":""},{default:t(()=>[u(r,{label:"HTTP\u7AEF\u53E3",prop:"HTTP",rules:{required:!0,message:"\u8BF7\u8F93\u5165HTTP\u7AEF\u53E3",trigger:"change"}},{default:t(()=>[u(a,{modelValue:e.value.HTTP,"onUpdate:modelValue":o[0]||(o[0]=n=>e.value.HTTP=n),placeholder:"\u8BF7\u8F93\u5165HTTP\u7AEF\u53E3"},null,8,["modelValue"])]),_:1}),u(r,{label:"RTSP\u7AEF\u53E3",prop:"RTSP",rules:{required:!0,message:"\u8BF7\u8F93\u5165RTSP\u7AEF\u53E3",trigger:"change"}},{default:t(()=>[u(a,{modelValue:e.value.RTSP,"onUpdate:modelValue":o[1]||(o[1]=n=>e.value.RTSP=n),placeholder:"\u8BF7\u8F93\u5165RTSP\u7AEF\u53E3"},null,8,["modelValue"])]),_:1}),u(r,{label:"SDK\u7AEF\u53E3",prop:"SDKSERVER",rules:{required:!0,message:"\u8BF7\u8F93\u5165SDK\u7AEF\u53E3",trigger:"change"}},{default:t(()=>[u(a,{modelValue:e.value.SDKSERVER,"onUpdate:modelValue":o[2]||(o[2]=n=>e.value.SDKSERVER=n),placeholder:"\u8BF7\u8F93\u5165SDK\u7AEF\u53E3"},null,8,["modelValue"])]),_:1}),u(r,null,{default:t(()=>[c(f)?(i(),p(B,{key:0,type:"primary",onClick:T},{default:t(()=>[D(" \u4FDD\u5B58 ")]),_:1})):V("",!0)]),_:1})]),_:1},8,["model"])])]),_:1})}}});const w=h(x,[["__scopeId","data-v-4c642fbc"]]);export{w as default};
