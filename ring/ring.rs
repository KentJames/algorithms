pub struct RingBufferData<T> {
    bytes: usize,
    dtype: usize,
    payload: Vec<T>,
}

pub enum RingError {
    NoError = 0,
    Error = -1}

pub struct RingBuffer<T> {
    pub capacity: u64,
    pub t_size: usize,

    data: Vec<RingBufferData<T>>,
    current_index: u64,
}

impl<T> RingBuffer<T> {
    pub fn init_ring_buffer(capacity: u64, dtype_size: usize) -> Self {

	let mut data_vec : Vec<RingBufferData<T>> = Vec::with_capacity(capacity as usize);
	for _i in 0..capacity {
	    let dummy_entry : RingBufferData<T> =
	    RingBufferData { bytes: 0, dtype: 0, payload: Vec::new(),};
	    data_vec.push(dummy_entry);
	}
	
	Self { capacity: capacity,
	       t_size: dtype_size,
	       data: data_vec,
	       current_index: 0, }
    }
    
    pub fn get_current_index(&self) -> u64 {
	self.current_index
    }

    pub fn get_data(&self) -> &RingBufferData<T> {
	let data = &self.data[self.current_index as usize];
	return data;
    }

    pub fn add_new_data(&mut self, data: Vec<T>) -> RingError {

	let err = RingError::NoError;
	
	let new_ring_buffer_data : RingBufferData<T> =
	RingBufferData { bytes: data.len() * self.t_size,
	  dtype: self.t_size,
	  payload: data, };
	self.increment_ring_index();

	self.data[self.current_index as usize] = new_ring_buffer_data;
	

	return err;
    }

    // Private Methods

    fn increment_ring_index(&mut self) -> RingError {
	let increment_index = self.current_index + 1;

	if increment_index >= self.capacity {

	    self.current_index = 0;
	} else {
	    self.current_index = increment_index;
	}
	
	return RingError::NoError;
    }
    
}

fn main(){


    let mut my_ring_buffer : RingBuffer<f64> = RingBuffer::init_ring_buffer(3, 8);

    let vec_1 = vec![0.,2.,4.,6.];
    let vec_2 = vec![1.,3.,5.,7.];
    let vec_3 = vec![2.,4.,6.,8.];
    let vec_4 = vec![3.,5.,7.,9.];

    my_ring_buffer.add_new_data(vec_1);
    println!("Current Ring Buffer Index: {}",my_ring_buffer.get_current_index());
    {
	let current_val = my_ring_buffer.get_data();
	println!("Ring Buffer bytes: {}, Data type size: {}",current_val.bytes, current_val.dtype);
	println!("Ring Buffer Data: {} {} {} {}",current_val.payload[0], current_val.payload[1], current_val.payload[2], current_val.payload[3]);
    }
    
    my_ring_buffer.add_new_data(vec_2);
    println!("Current Ring Buffer Index: {}",my_ring_buffer.get_current_index());

    {
	let current_val = my_ring_buffer.get_data();
	println!("Ring Buffer Data: {} {} {} {}",current_val.payload[0], current_val.payload[1], current_val.payload[2], current_val.payload[3]);
    }

    my_ring_buffer.add_new_data(vec_3);
    println!("Current Ring Buffer Index: {}",my_ring_buffer.get_current_index());
    {
	let current_val = my_ring_buffer.get_data();
	println!("Ring Buffer Data: {} {} {} {}",current_val.payload[0], current_val.payload[1], current_val.payload[2], current_val.payload[3]);
    }

    my_ring_buffer.add_new_data(vec_4);
    println!("Current Ring Buffer Index: {}",my_ring_buffer.get_current_index());
    {
	let current_val = my_ring_buffer.get_data();
	println!("Ring Buffer Data: {} {} {} {}",current_val.payload[0], current_val.payload[1], current_val.payload[2], current_val.payload[3]);
    }

    
    
}
