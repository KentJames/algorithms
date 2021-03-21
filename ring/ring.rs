pub struct RingBufferData<T> {
    bytes: usize,
    dtype: usize,
    payload: Vec<T>,
}

#[derive(PartialEq)]
pub enum RingError {
    NoError = 0,
    Error = -1,
    RingEmptyError = -10,
}

pub struct RingBufferPayloadRead<'a, T>{
    status: RingError,
    ring_buffer_data: &'a RingBufferData<T>,
}

pub struct RingBuffer<T> {
    pub capacity: u64,
    pub t_size: usize,

    data: Vec<RingBufferData<T>>,
    items_in_buffer:    u64,
    current_head_index: u64,
    current_read_index: u64,
}

fn increment_index( index: u64, capacity: u64 ) -> u64 {
	
    if index + 1 >= capacity {
	return 0;
    } else {
	return index + 1;
    }
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
	       items_in_buffer: 0,
	       current_head_index: 0,
	       current_read_index: 0}
    }
    
    pub fn get_current_head_index(&self) -> u64 {
	self.current_head_index
    }

    pub fn get_data(&mut self) -> RingBufferPayloadRead<T> {
	// This should always be something even if ring buffer is empty.
	let data = &mut self.data[self.current_read_index as usize];
	let mut return_error = RingError::NoError;
	if self.items_in_buffer==0 {
	    return_error = RingError::Error;
	}
	
	let payload_return : RingBufferPayloadRead<T> =
	RingBufferPayloadRead { status: return_error,
				ring_buffer_data: data };
	self.current_read_index = increment_index(self.current_read_index, self.capacity);
	if self.items_in_buffer > 0 {
	    self.items_in_buffer -= 1;
	}
	
	
	return payload_return;
    }

    pub fn add_new_data(&mut self, data: Vec<T>) -> RingError {

	let err = RingError::NoError;
	
	let new_ring_buffer_data : RingBufferData<T> =
	RingBufferData { bytes: data.len() * self.t_size,
	  dtype: self.t_size,
	  payload: data, };	

	self.data[self.current_head_index as usize] = new_ring_buffer_data;
	self.current_head_index = increment_index(self.current_head_index, self.capacity);

	if self.items_in_buffer == self.capacity {
	    self.current_read_index = increment_index(self.current_read_index, self.capacity);
	}
	
	if self.items_in_buffer < self.capacity {
	    self.items_in_buffer += 1;
	}


	return err;
    }

    
}

fn main(){


    let mut my_ring_buffer : RingBuffer<f64> = RingBuffer::init_ring_buffer(3, 8);

    let vec_1 = vec![0.,2.,4.,6.];
    let vec_2 = vec![1.,3.,5.,7.];
    let vec_3 = vec![2.,4.,6.,8.];
    let vec_4 = vec![3.,5.,7.,9.];

    my_ring_buffer.add_new_data(vec_1);

    
    
    
    println!("Current Ring Buffer Index: {}",my_ring_buffer.get_current_head_index());
    {
	let current_val = my_ring_buffer.get_data();
	if current_val.status==RingError::NoError  {
	    println!("Ring Buffer bytes: {}, Data type size: {}",current_val.ring_buffer_data.bytes, current_val.ring_buffer_data.dtype);
	    println!("Ring Buffer Data: {} {} {} {}",current_val.ring_buffer_data.payload[0], current_val.ring_buffer_data.payload[1], current_val.ring_buffer_data.payload[2], current_val.ring_buffer_data.payload[3]);
	} else {
	    println!("Ring Error!");
	}
    }
    
    my_ring_buffer.add_new_data(vec_2);
    println!("Current Ring Buffer Index: {}",my_ring_buffer.get_current_head_index());

    {
	let current_val = my_ring_buffer.get_data();
	if current_val.status==RingError::NoError  {
	    println!("Ring Buffer bytes: {}, Data type size: {}",current_val.ring_buffer_data.bytes, current_val.ring_buffer_data.dtype);
	    println!("Ring Buffer Data: {} {} {} {}",current_val.ring_buffer_data.payload[0], current_val.ring_buffer_data.payload[1], current_val.ring_buffer_data.payload[2], current_val.ring_buffer_data.payload[3]);
	} else {
	    println!("Ring Error!");
	}
    }

    my_ring_buffer.add_new_data(vec_3);
    println!("Current Ring Buffer Index: {}",my_ring_buffer.get_current_head_index());
    {
	let current_val = my_ring_buffer.get_data();
	if current_val.status==RingError::NoError  {
	    println!("Ring Buffer bytes: {}, Data type size: {}",current_val.ring_buffer_data.bytes, current_val.ring_buffer_data.dtype);
	    println!("Ring Buffer Data: {} {} {} {}",current_val.ring_buffer_data.payload[0], current_val.ring_buffer_data.payload[1], current_val.ring_buffer_data.payload[2], current_val.ring_buffer_data.payload[3]);
	} else {
	    println!("Ring Error!");
	}
    }

    my_ring_buffer.add_new_data(vec_4);
    println!("Current Ring Buffer Index: {}",my_ring_buffer.get_current_head_index());
    {
	let current_val = my_ring_buffer.get_data();
	if current_val.status==RingError::NoError  {
	    println!("Ring Buffer bytes: {}, Data type size: {}",current_val.ring_buffer_data.bytes, current_val.ring_buffer_data.dtype);
	    println!("Ring Buffer Data: {} {} {} {}",current_val.ring_buffer_data.payload[0], current_val.ring_buffer_data.payload[1], current_val.ring_buffer_data.payload[2], current_val.ring_buffer_data.payload[3]);
	} else {
	    println!("Ring Error!");
	}
    }

    
    
}

