import{i as j,j as c,k as U,s as _,l as $,n as q,p as x,q as M,v as o,y as N,x as d,z as J,A as v,B as K}from"./index-98552006.js";const Q=j({id:"pure-app",state:()=>({sidebar:{opened:c().SidebarStatus,withoutAnimation:!1,isClickCollapse:!1},layout:c().Layout,device:U()?"mobile":"desktop"}),getters:{getSidebarStatus(){return this.sidebar.opened},getDevice(){return this.device}},actions:{TOGGLE_SIDEBAR(t,n){},async toggleSideBar(t,n){await this.TOGGLE_SIDEBAR(t,n)},toggleDevice(t){this.device=t},setLayout(t){this.layout=t}}});function V(){return Q(_)}const y="\u5F53\u524D\u8DEF\u7531\u914D\u7F6E\u4E0D\u6B63\u786E\uFF0C\u8BF7\u68C0\u67E5\u914D\u7F6E";function X(){var h,g;const t=$(),n=V(),E=q().options.routes,{wholeMenus:m}=x(M()),D=(g=(h=c())==null?void 0:h.TooltipEffect)!=null?g:"light",f=o(()=>{var e;return(e=d())==null?void 0:e.username}),A=o(()=>{var e;return((e=d())==null?void 0:e.iUserLevel)==0}),B=o(()=>f.value?{marginRight:"10px"}:""),C=o(()=>!n.getSidebarStatus),R=o(()=>"pc"),{$storage:a,$config:T}=N(),b=o(()=>{var e;return(e=a==null?void 0:a.layout)==null?void 0:e.layout}),k=o(()=>T.Title);function F(e){const r=c().Title;r?document.title=`${e.title} | ${r}`:document.title=e.title}function L(){d().logOut()}function I(){J.push("/welcome")}function w(){v.emit("openPanel")}function z(){n.toggleSideBar()}function G(e){e==null||e.handleResize()}function H(e){var u;if(!e.children)return console.error(y);const r=/^http(s?):\/\//,s=(u=e.children[0])==null?void 0:u.path;return r.test(s)?e.path+"/"+s:s}function O(e,r){if(m.value.length===0||P(e))return;let s="";const u=e.lastIndexOf("/");u>0&&(s=e.slice(0,u));function l(p,S){return S?S.map(i=>{i.path===p?i.redirect?l(i.redirect,i.children):v.emit("changLayoutRoute",{indexPath:p,parentPath:s}):i.children&&l(p,i.children)}):console.error(y)}l(e,r)}function P(e){return K.includes(e)}return{route:t,title:k,device:R,layout:b,logout:L,routers:E,$storage:a,backHome:I,onPanel:w,changeTitle:F,toggleSideBar:z,menuSelect:O,handleResize:G,resolvePath:H,isCollapse:C,pureApp:n,username:f,isAdmin:A,avatarsStyle:B,tooltipEffect:D}}export{V as a,X as u};