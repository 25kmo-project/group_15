const db = require('../routes/database');
const bcrypt = require('bcrypt');
const saltRounds = 10;

const card={
//Get all cards
     getAll: function(callback){
        return db.query('SELECT * FROM card', callback);
},
//Get single card by id
    getOne: function(card_id, callback){
        return db.query ("select * FROM card WHERE card_id=?", [card_id] , callback);
    },
 
//Add new card (PIN is hashed before storing)
  add: function(newCard, callback) {
    bcrypt.hash(newCard.pin_code, saltRounds, function(err, hash) {
      if (err) return callback(err);

      return db.query(
        'INSERT INTO card (card_number, pin_code, card_type, user_id, status) VALUES (?,?,?,?,?)',
        [newCard.card_number, hash, newCard.card_type, newCard.user_id, newCard.status],
        callback
      );
    });
  },
//Update card details by id
    update: function(updatedCard, card_id, callback){
        return db.query ("UPDATE card SET card_number=?, card_type=?, status=?, user_id=? WHERE card_id=?", 
        [updatedCard.card_number, updatedCard.card_type, updatedCard.status, updatedCard.user_id, card_id], callback);
    },
    //Update pin (always hashed before storing)
  updatePin: function(card_id, newPin, callback) {
    bcrypt.hash(newPin, saltRounds, function(err, hash) {
      if (err) return callback(err);

      return db.query(
        'UPDATE card SET pin_code = ? WHERE card_id = ?',
        [hash, card_id],
        callback
      );
    });
  },
//Delete card by id
    delete: function(card_id, callback){
        return db.query ("DELETE FROM card WHERE card_id=?", [card_id], callback);
    }
};
module.exports=card;