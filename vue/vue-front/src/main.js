
// app 
import { createApp } from 'vue'
import App from './App.vue'

// element-plus-ui
import ElementPlus from 'element-plus'
import 'element-plus/dist/index.css'

// router
import router from './util/router.js'


const app = createApp(App)
app.use(ElementPlus)
app.use(router)
app.mount('#app')