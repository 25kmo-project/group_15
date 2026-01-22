//haetaan tietokantayhteys database.js-tiedostosta
const db = require('../routes/database');


const card={

     getAll: function(callback){
        return db.query('SELECT * FROM card', callback);
},
//saadaan yhden cardin tiedot
    getOne: function(card_id, callback){
        return db.query ("select * FROM card WHERE card_id=?", [card_id] , callback);
    },
 //hae card korttinumerolla 
    getByNumber: function(card_number, callback){
       return db.query ("select * FROM card WHERE card_number=?",[card_number] , callback);

    },
//lisätään uusi card
    add: function(newCard, callback){
        return db.query ("INSERT INTO card (card_id, card_number, pin_code, status, user_id) VALUES (?,?,?,?,?)",
            [newCard.card_id, newCard.card_number, newCard.pin_code, newCard.status, newCard.user_id], callback);
    },
//päivitä card tietoja
    update: function(updatedCard, card_id, callback){
        return db.query ("UPDATE card SET card_number=?, pin_code=?, status=?, user_id=? WHERE card_id=?", 
        [updatedCard.card_number, updatedCard.pin_code, updatedCard.status, updatedCard.user_id, card_id], callback);
    },
//poista card
    delete: function(card_id, callback){
        return db.query ("DELETE FROM card WHERE card_id=?", [card_id], callback);
    }
};
module.exports=card;