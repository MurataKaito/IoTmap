<template>
  <div id=app>
  </div>
</template>
<script>

import 'leaflet/dist/leaflet.css'
import L from 'leaflet'
import axios from 'axios'

export default {
  data() {
    return {
      map: null,
      marker: null,
      redIcon: null, // 赤色のマーカーアイコン
      blueIcon: null, // 青色のマーカーアイコン

      personCountMarker: null // 人数に基づいて表示するマーカー
    }
  },
  mounted() {
    this.map = L.map('app', { center: L.latLng(34.70010032717841, 135.49305348253878), zoom: 15 }).addLayer(
      L.tileLayer('http://{s}.tile.osm.org/{z}/{x}/{y}.png')
    )
    // 定期的に人数を取得
    setInterval(() => {
      this.fetchPersonCount();
    }, 5000);  // 5秒ごとに実行

    // 赤色のマーカーアイコンを作成
    this.redIcon = new L.Icon({
      iconUrl: '/images/marker-icon-2x-red.png',
      shadowUrl: '/images/marker-shadow.png',
      iconSize: [25, 41],
      iconAnchor: [12, 41],
      popupAnchor: [1, -34],
      shadowSize: [41, 41]
    });



    // 青色のマーカーアイコンを作成
    this.blueIcon = new L.Icon({
      iconUrl: '/images/marker-icon-2x.png',
      shadowUrl: '/images/marker-shadow.png',
      iconSize: [25, 41],
      iconAnchor: [12, 41],
      popupAnchor: [1, -34],
      shadowSize: [41, 41]
    });


    // 指定した位置に赤色のマーカーを追加
    const locations = [
      //[34.70010032717841, 135.49305348253878],
      [34.655953, 135.502269],
      [34.666517, 135.502186],
      [34.672825, 135.520323],
      [34.669786, 135.501534],
      [34.694552, 135.497325], // 大阪市役所
      [34.699353, 135.52057],  // ドーンセンター
      [34.66546, 135.516361],  // HEP FIVE
      [34.665273, 135.502062], // グランフロント大阪
      [34.672409, 135.505965], // なんばCITY
      [34.668875, 135.502842], // 難波駅
      [34.668522, 135.501484], // 複合施設あべのキューズタウン
      [34.66694, 135.501601],  // 大阪府立国際会議場
      [34.669786, 135.498084], // 梅田スカイビル
      [34.670418, 135.49932],  // 大阪駅
      [34.70275, 135.49489],
      [35.691778, 139.71125],
      [34.704933, 135.512136]
    ];

  locations.forEach(([lat, lng]) => {
    L.marker([lat, lng], {icon: this.redIcon}).addTo(this.map);
  });

    // カスタムコントロール（ボタン）を地図上に追加
    //this.addCustomControl(this.map);
    this.updateLocation(blueIcon)
    this.fetchPersonCount(redIcon);
  },

  methods: {
    async updateLocation(icon) {
      try {
        const response = await axios.get('http://172.20.10.2/location')
        const lat = response.data.lat
        const lng = response.data.lng
        if (this.marker) {
          this.marker.setLatLng([lat, lng])
        } else {
          this.marker = L.marker([lat, lng], {icon: icon}).addTo(this.map)
        }
      } catch (error) {
        console.error(error)
      }
    },
  
    // サーバーから人の数を取得して処理する新しいメソッド
    async fetchPersonCount(){
      try {
        const response = await axios.get('http://100.127.255.249:5000/get_count');
        const personCount = response.data.person_count;
        console.log("Fetched person count:", personCount); // レスポンスをコンソールに出力

        // 2人未満の場合は青色、2人以上の場合は赤色アイコンを使用
        let selectedIcon = (personCount < 1) ? this.redIcon : this.blueIcon;
        
        // マーカーを更新または新規作成
        if (this.personCountMarker) {
        this.personCountMarker.setIcon(selectedIcon);
        } else {
        this.personCountMarker = L.marker([34.70010032717841, 135.49305348253878], { icon: selectedIcon }).addTo(this.map);
        }
        this.$forceUpdate(); // Vueインスタンスを強制的に更
      } catch (error) {
          console.error('Error fetching person count:', error);
          if (error.response) {
            console.error('Response data:', error.response.data);
            console.error('Response status:', error.response.status);
          } else if (error.request) {
            console.error('No response received:', error.request);
          } else {
            console.error('Error setting up request:', error.message);
          }
    
      


        }
    },
    // 地図上にカスタムコントロール（ボタン）を追加する関数
    //addCustomControl: function(map) {
      //const customControl = L.Control.extend({
        //options: {
          //position: 'topright' // コントロールの位置を指定
        //},
        //onAdd: function (map) {
          //const container = L.DomUtil.create('div', 'leaflet-bar leaflet-control leaflet-control-custom');
          //container.innerHTML = '<button>マーカーを表示</button>';
          //container.style.backgroundColor = 'white';
          //container.style.width = 'auto';
          //container.style.height = 'auto';

          //container.onclick = () => {
            // ここにボタンクリック時の処理を書く
            // 例: 特定の場所にマーカーを表示
            //L.marker([34.70010032717841, 135.49305348253878]).addTo(map);
          //};

          //return container;
        //}
      //});

      //map.addControl(new customControl());
    //}

  },

};
</script>


<style>
#app {
  height: 100vh;
  width: 100%;
}
</style>