import { BrowserWindow, app } from 'electron';
import { once } from 'events';
import path from 'path';

import { OSCManager } from './oscManager';
import { Cursor2D } from './oscReceiver';

import '../vendor/oneDollar';

async function init(): Promise<[OSCManager, any[]]> {
    return Promise.all([
        new Promise<OSCManager>((resolve) => {
            const manager = new OSCManager();
            manager.on('ready', () => { resolve(manager); })
        }),
        once(app, 'ready')
    ]);
}

async function main() {
    const [manager, _] = await init()

    const mainWindow = await new Promise<BrowserWindow>(async (resolve) => {
        const mainWindow = new BrowserWindow({ width: 600, height: 400 })

        const indexHTML = path.join(`${__dirname}/resources/index.html`)
        await mainWindow.loadFile(indexHTML)

        resolve(mainWindow)
    })


    manager.on("update", (cursors :Cursor2D[]) => {
        const content = mainWindow.webContents;

        var tableData: string;

        if(cursors.length <= 0) {
            tableData = "";
        } else {
            tableData = cursors.map(cursor => `\
<tr>\
    <td>${cursor.sessionID}</td>\
    <td>${cursor.position.x}</td>\
    <td>${cursor.position.y}</td>\
    <td>${cursor.velocity.x}</td>\
    <td>${cursor.velocity.y}</td>\
    <td>${cursor.accelleration}</td>\
</tr>\
`).join('')
        }


        const dynamicTableBody = `\
<table>\
    <thead>\
        <tr>\
            <th>ID</th>\
            <th>Position X</td>\
            <td>Position Y</td>\
            <th>Velocity X</td>\
            <td>Velocity Y</td>\
            <th>Accelleration</th>\
        <tr>\
    </thead>\
    <tbody>\
        ${tableData}\
    </tbody>\
</table>\
`

        content.executeJavaScript(`\
document.getElementById('finger_table_wrapper').innerHTML = '${dynamicTableBody}';
        `);

    })

}

main();
